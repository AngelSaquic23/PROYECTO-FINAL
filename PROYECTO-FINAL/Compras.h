#pragma once
#include <iostream>
#include <string>
#include <mysql.h>
#include <conio.h>
#include <vector> 
#include <tuple>
#include <sstream>
#include <windows.h>
#include <utility>
#include <iomanip>
#include "Conexionsql.h"
using namespace std;

class Compras {
private:
    int no_orden_compra, idProveedor;
    string fecha_orden;
    string fechaingreso; // Agregado aquí

public:
    Compras() : no_orden_compra(0), idProveedor(0), fecha_orden(""), fechaingreso("") {}

    Compras(int no_orden, int id_prov, string fecha_ord, string fecha_ing) {
        no_orden_compra = no_orden;
        idProveedor = id_prov;
        fecha_orden = fecha_ord;
        fechaingreso = fecha_ing;
    }

    void crear() {

        conexion cn = conexion();
        cn.abrir_conexion();

        if (cn.getConectar()) {
            gotoxy(5, 2); cout << "Ingrese ID Proveedor: ";
            gotoxy(35, 2); cin >> idProveedor;

            cin.ignore();
            gotoxy(5, 4); cout << "Ingrese Fecha de Orden (YYYY-MM-DD): ";
            gotoxy(45, 4); getline(cin, fecha_orden);

            gotoxy(5, 6); cout << "Ingrese Fecha de Ingreso (YYYY-MM-DD HH:MM:SS): ";
            gotoxy(55, 6); getline(cin, fechaingreso);

            gotoxy(5, 8); cout << "Presione ENTER 2 veces para guardar...";
            int enterCount = 0;
            while (enterCount < 2) {
                if (_getch() == 13) {
                    enterCount++;
                }
                else {
                    enterCount = 0;
                }
            }

            // Generar nuevo número de orden automáticamente
            string query_max = "SELECT IFNULL(MAX(no_orden_compra), 0) + 1 FROM compras;";
            const char* q_max = query_max.c_str();
            MYSQL_RES* res_max;
            MYSQL_ROW row_max;

            int estado_max = mysql_query(cn.getConectar(), q_max);
            if (!estado_max) {
                res_max = mysql_store_result(cn.getConectar());
                row_max = mysql_fetch_row(res_max);
                if (row_max) {
                    no_orden_compra = atoi(row_max[0]);
                }
                mysql_free_result(res_max);
            }

            string insert = "INSERT INTO compras(no_orden_compra, idProveedor, fecha_orden, fechaingreso) VALUES (" +
                to_string(no_orden_compra) + ", " + to_string(idProveedor) + ", '" + fecha_orden + "', '" + fechaingreso + "');";
            const char* i = insert.c_str();
            int q_estado = mysql_query(cn.getConectar(), i);

            if (!q_estado) {
                cout << "\nCompra registrada exitosamente.\n";

                unsigned long long idCompra = mysql_insert_id(cn.getConectar());

                int idProducto, cantidad;
                float precio_costo_unitario;
                char opcion = 's';

                while (opcion == 's' || opcion == 'S') {
                    system("cls"); // Limpia la pantalla
                    cout << "\nIngrese ID Producto:\n>> ";
                    cin >> idProducto;
                    cout << "Ingrese Cantidad:\n>> ";
                    cin >> cantidad;
                    cout << "Ingrese Precio Costo Unitario:\n>> ";
                    cin >> precio_costo_unitario;

                    string insert_det = "INSERT INTO compras_detalle(idCompra, idProducto, cantidad, precio_costo_unitario) VALUES (" +
                        to_string(idCompra) + ", " + to_string(idProducto) + ", " + to_string(cantidad) + ", " + to_string(precio_costo_unitario) + ");";
                    const char* i_det = insert_det.c_str();
                    q_estado = mysql_query(cn.getConectar(), i_det);

                    if (!q_estado) {
                        cout << "Detalle agregado.\n";

                        // Actualizar existencia, precio costo y fecha ingreso del producto
                        string update_producto = "UPDATE productos SET existencia = existencia + " + to_string(cantidad) +
                            ", precio_costo = " + to_string(precio_costo_unitario) +
                            ", fecha_ingreso = '" + fechaingreso + "'" +
                            " WHERE idProducto = " + to_string(idProducto) + ";";

                        const char* u_producto = update_producto.c_str();
                        q_estado = mysql_query(cn.getConectar(), u_producto);

                        if (!q_estado) {
                            cout << "Producto actualizado exitosamente.\n";
                        }
                        else {
                            cout << "Error al actualizar producto.\n";
                        }

                    }
                    else {
                        cout << "Error al agregar detalle.\n";
                    }

                    cout << "¿Desea agregar otro producto? (s/n): ";
                    cin >> opcion;
                }

                // Mostrar número de orden con formato después de agregar productos
                stringstream ss;
                ss << setw(5) << setfill('0') << no_orden_compra;
                cout << "\nNo. Orden generado: " << ss.str() << endl;

            }
            else {
                cout << "\nError al registrar la compra.\n";
            }
        }
        else {
            cout << "Error en la conexión.\n";
        }

        cn.cerrar_conexion();
    }

    void leer() {
        conexion cn = conexion();
        MYSQL_ROW fila;
        MYSQL_RES* resultado;

        cn.abrir_conexion();
        if (cn.getConectar()) {
            string consulta =
                "SELECT c.idCompra, c.no_orden_compra, p.proveedor, c.fecha_orden, c.fechaingreso, "
                "pr.producto, cd.cantidad, cd.precio_costo_unitario "
                "FROM compras c "
                "INNER JOIN proveedores p ON c.idProveedor = p.idProveedor "
                "INNER JOIN compras_detalle cd ON c.idCompra = cd.idCompra "
                "INNER JOIN productos pr ON cd.idProducto = pr.idProducto "
                "ORDER BY c.idCompra, pr.producto";

            const char* c = consulta.c_str();
            int q_estado = mysql_query(cn.getConectar(), c);

            if (!q_estado) {
                resultado = mysql_store_result(cn.getConectar());

                cout << left
                    << setw(10) << "ID"
                    << setw(15) << "No Orden"
                    << setw(25) << "Proveedor"
                    << setw(20) << "Fecha Orden"
                    << setw(20) << "Fecha Ingreso"
                    << setw(30) << "Producto"
                    << setw(10) << "Cantidad"
                    << setw(15) << "Precio Costo" << endl;

                cout << string(145, '-') << endl;

                while ((fila = mysql_fetch_row(resultado))) {
                    cout << left
                        << setw(10) << fila[0]
                        << setw(15) << fila[1]
                        << setw(25) << fila[2]
                        << setw(20) << fila[3]
                        << setw(20) << fila[4]
                        << setw(30) << fila[5]
                        << setw(10) << fila[6]
                        << setw(15) << fila[7] << endl;
                }
            }
            else {
                cout << "Error al consultar compras.\n";
            }
        }
        else {
            cout << "Error en la conexión.\n";
        }
        cn.cerrar_conexion();
    }

    void actualizar() {
        conexion cn = conexion();
        cn.abrir_conexion();

        if (!cn.getConectar()) {
            cout << "Error en la conexion.\n";
            return;
        }

        while (true) {
            int no_orden_compra;
            cout << "\nIngrese el numero de orden de compra a actualizar: ";
            cin >> no_orden_compra;
            cin.ignore();

            string consulta =
                "SELECT c.idCompra, c.no_orden_compra, p.proveedor, c.fecha_orden, c.fechaingreso, c.idProveedor "
                "FROM compras c "
                "INNER JOIN proveedores p ON c.idProveedor = p.idProveedor "
                "WHERE c.no_orden_compra = " + to_string(no_orden_compra) + ";";

            if (mysql_query(cn.getConectar(), consulta.c_str()) == 0) {
                MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
                MYSQL_ROW fila = mysql_fetch_row(resultado);

                if (fila) {
                    int idCompra = atoi(fila[0]);
                    string old_no_orden = fila[1];
                    string old_proveedor = fila[2];
                    string old_fecha_orden = fila[3];
                    string old_fecha_ingreso = fila[4];
                    string old_idProveedor = fila[5];
                    mysql_free_result(resultado);

                    // Obtener la fecha_orden más reciente
                    string consulta_fecha_reciente = "SELECT MAX(fecha_orden) FROM compras;";
                    string fecha_mas_reciente;
                    if (mysql_query(cn.getConectar(), consulta_fecha_reciente.c_str()) == 0) {
                        MYSQL_RES* res_fecha = mysql_store_result(cn.getConectar());
                        MYSQL_ROW row_fecha = mysql_fetch_row(res_fecha);
                        if (row_fecha && row_fecha[0]) {
                            fecha_mas_reciente = row_fecha[0];
                        }
                        mysql_free_result(res_fecha);
                    }

                    bool es_mas_reciente = (old_fecha_orden == fecha_mas_reciente);
                    system("cls"); // Limpia la pantalla
                    cout << "\nDatos actuales de la compra:\n";
                    cout << left << setw(10) << "ID" << setw(15) << "No Orden"
                        << setw(25) << "Proveedor" << setw(20) << "Fecha Orden"
                        << setw(20) << "Fecha Ingreso" << endl;
                    cout << string(90, '-') << endl;
                    cout << left << setw(10) << idCompra
                        << setw(15) << old_no_orden
                        << setw(25) << old_proveedor
                        << setw(20) << old_fecha_orden
                        << setw(20) << old_fecha_ingreso << endl;

                    cout << "\nNumero de Orden de Compra (no editable): " << old_no_orden << endl;

                    if (!es_mas_reciente) {
                        cout << "\nEsta orden no es la mas reciente, no se permiten cambios en las fechas.\n";
                    }

                    char confirmar_cabecera;
                    cout << "\nDesea actualizar esta compra? (s/n): ";
                    cin >> confirmar_cabecera;
                    cin.ignore();
                    if (confirmar_cabecera != 's' && confirmar_cabecera != 'S') {
                        char otra;
                        system("cls"); // Limpia la pantalla
                        cout << "\nDesea modificar otra orden? (s/n): ";
                        cin >> otra;
                        cin.ignore();
                        if (otra != 's' && otra != 'S') break;
                        else continue;
                    }

                    string new_idProveedor;
                    system("cls"); // Limpia la pantalla
                    cout << "Nuevo ID de Proveedor (" << old_idProveedor << "): ";
                    getline(cin, new_idProveedor);
                    if (new_idProveedor.empty()) new_idProveedor = old_idProveedor;

                    string new_fecha_orden = old_fecha_orden;
                    string new_fecha_ingreso = old_fecha_ingreso;

                    if (es_mas_reciente) {
                        cout << "Nueva Fecha de Orden (" << old_fecha_orden << ") [YYYY-MM-DD]: ";
                        getline(cin, new_fecha_orden);
                        if (new_fecha_orden.empty()) new_fecha_orden = old_fecha_orden;

                        cout << "Nueva Fecha de Ingreso (" << old_fecha_ingreso << ") [YYYY-MM-DD]: ";
                        getline(cin, new_fecha_ingreso);
                        if (new_fecha_ingreso.empty()) new_fecha_ingreso = old_fecha_ingreso;
                    }
                    else {
                        cout << "Fechas no modificadas.\n";
                    }

                    string consulta_detalle =
                        "SELECT cd.idCompra_detalle, cd.idProducto, pr.producto, cd.cantidad, cd.precio_costo_unitario "
                        "FROM compras_detalle cd "
                        "INNER JOIN productos pr ON cd.idProducto = pr.idProducto "
                        "WHERE cd.idCompra = " + to_string(idCompra) + ";";

                    if (mysql_query(cn.getConectar(), consulta_detalle.c_str()) == 0) {
                        MYSQL_RES* res_det = mysql_store_result(cn.getConectar());
                        MYSQL_ROW row_det;

                        vector<tuple<int, int, string, int, float>> productos;
                        system("cls"); // Limpia la pantalla
                        cout << "\nProductos actuales:\n";
                        cout << left
                            << setw(10) << "IDDet"
                            << setw(10) << "IDProd"
                            << setw(30) << "Producto"
                            << setw(10) << "Cantidad"
                            << setw(15) << "Costo Unitario" << endl;
                        cout << string(80, '-') << endl;

                        while ((row_det = mysql_fetch_row(res_det))) {
                            int idDetalle = atoi(row_det[0]);
                            int idProd = atoi(row_det[1]);
                            string nombre = row_det[2];
                            int cantidad = atoi(row_det[3]);
                            float costo = static_cast<float>(atof(row_det[4]));

                            productos.push_back(make_tuple(idDetalle, idProd, nombre, cantidad, costo));

                            cout << left << setw(10) << idDetalle
                                << setw(10) << idProd
                                << setw(30) << nombre
                                << setw(10) << cantidad
                                << setw(15) << fixed << setprecision(2) << costo << endl;
                        }
                        mysql_free_result(res_det);

                        cout << "\n--- Modificacion de productos ---\n";
                        for (auto& prod : productos) {
                            int idDetalle = get<0>(prod);
                            int idProd = get<1>(prod);
                            string nombre = get<2>(prod);
                            int old_cantidad = get<3>(prod);
                            float old_costo = get<4>(prod);

                            string input;
                            cout << "\nProducto: " << nombre << " (ID: " << idProd << ", Detalle: " << idDetalle << ")";

                            cout << "\nCantidad actual (" << old_cantidad << "), nueva cantidad (Enter para mantener): ";
                            getline(cin, input);
                            int nueva_cantidad = input.empty() ? old_cantidad : stoi(input);

                            cout << "Costo unitario actual (" << old_costo << "), nuevo costo (Enter para mantener): ";
                            getline(cin, input);
                            float nuevo_costo = input.empty() ? old_costo : stof(input);

                            get<3>(prod) = nueva_cantidad;
                            get<4>(prod) = nuevo_costo;
                        }

                        char confirmar;
                        cout << "\nDesea guardar todos los cambios? (s/n): ";
                        cin >> confirmar;
                        cin.ignore();

                        if (confirmar == 's' || confirmar == 'S') {
                            string update_compra =
                                "UPDATE compras SET idProveedor = " + new_idProveedor +
                                ", fecha_orden = '" + new_fecha_orden +
                                "', fechaingreso = '" + new_fecha_ingreso + "' "
                                "WHERE idCompra = " + to_string(idCompra) + ";";

                            bool exito_cabecera = mysql_query(cn.getConectar(), update_compra.c_str()) == 0;

                            bool exito_detalle = true;
                            for (auto& prod : productos) {
                                int idDetalle = get<0>(prod);
                                int idProd = get<1>(prod);
                                int nueva_cantidad = get<3>(prod);
                                float nuevo_costo = get<4>(prod);

                                int cantidad_anterior = 0;
                                string consulta_cantidad =
                                    "SELECT cantidad FROM compras_detalle WHERE idCompra_detalle = " + to_string(idDetalle) + ";";

                                if (mysql_query(cn.getConectar(), consulta_cantidad.c_str()) == 0) {
                                    MYSQL_RES* res_cant = mysql_store_result(cn.getConectar());
                                    MYSQL_ROW row = mysql_fetch_row(res_cant);
                                    if (row) cantidad_anterior = atoi(row[0]);
                                    mysql_free_result(res_cant);
                                }

                                int diferencia = nueva_cantidad - cantidad_anterior;

                                string update_det =
                                    "UPDATE compras_detalle SET cantidad = " + to_string(nueva_cantidad) +
                                    ", precio_costo_unitario = " + to_string(nuevo_costo) +
                                    " WHERE idCompra_detalle = " + to_string(idDetalle) + ";";

                                if (mysql_query(cn.getConectar(), update_det.c_str()) != 0) {
                                    exito_detalle = false;
                                    continue;
                                }

                                string update_producto =
                                    "UPDATE productos SET existencia = existencia + " + to_string(diferencia) +
                                    ", precio_costo = " + to_string(nuevo_costo) +
                                    ", fecha_ingreso = '" + new_fecha_ingreso + "' "
                                    "WHERE idProducto = " + to_string(idProd) + ";";

                                if (mysql_query(cn.getConectar(), update_producto.c_str()) != 0) {
                                    exito_detalle = false;
                                }
                            }

                            if (exito_cabecera && exito_detalle) {
                                cout << "\nCompra actualizada correctamente.\n";
                            }
                            else {
                                cout << "\nError al actualizar la compra o sus productos.\n";
                            }
                        }
                        else {
                            cout << "Actualizacion cancelada.\n";
                        }
                    }
                    else {
                        cout << "Error al consultar productos de la compra.\n";
                    }
                }
                else {
                    cout << "No se encontro ninguna compra con ese numero de orden.\n";
                    mysql_free_result(resultado);
                }
            }
            else {
                cout << "Error al ejecutar la consulta.\n";
            }

            char repetir;
            cout << "\nDesea actualizar otra orden? (s/n): ";
            cin >> repetir;
            cin.ignore();
            if (repetir != 's' && repetir != 'S') break;
        }

        cn.cerrar_conexion();
    }

    void eliminar() {
        conexion cn = conexion();
        cn.abrir_conexion();

        if (!cn.getConectar()) {
            cout << "Error en la conexión." << endl;
            return;
        }

        while (true) {
            int no_orden;
            cout << "Ingrese el número de orden de compra a eliminar: ";
            cin >> no_orden;
            cin.ignore();

            string consulta_idCompra = "SELECT idCompra FROM compras WHERE no_orden_compra = " + to_string(no_orden) + ";";
            if (mysql_query(cn.getConectar(), consulta_idCompra.c_str()) != 0) {
                cout << "Error al ejecutar la consulta." << endl;
                break;
            }

            MYSQL_RES* res_id = mysql_store_result(cn.getConectar());
            if (mysql_num_rows(res_id) == 0) {
                cout << "No se encontró ninguna compra con ese número de orden." << endl;
                mysql_free_result(res_id);
            }
            else {
                MYSQL_ROW fila_id = mysql_fetch_row(res_id);
                int idCompra = atoi(fila_id[0]);
                mysql_free_result(res_id);

                // Mostrar datos de la compra con detalles
                string consulta =
                    "SELECT c.idCompra, c.no_orden_compra, p.proveedor, c.fecha_orden, c.fechaingreso, "
                    "pr.producto, cd.cantidad, cd.precio_costo_unitario "
                    "FROM compras c "
                    "INNER JOIN proveedores p ON c.idProveedor = p.idProveedor "
                    "INNER JOIN compras_detalle cd ON c.idCompra = cd.idCompra "
                    "INNER JOIN productos pr ON cd.idProducto = pr.idProducto "
                    "WHERE c.idCompra = " + to_string(idCompra) + " ORDER BY pr.producto;";

                if (mysql_query(cn.getConectar(), consulta.c_str()) == 0) {
                    MYSQL_RES* datos_compra = mysql_store_result(cn.getConectar());
                    MYSQL_ROW fila;

                    cout << "\nDatos de la compra a eliminar:\n";
                    cout << left
                        << setw(10) << "ID"
                        << setw(15) << "No Orden"
                        << setw(25) << "Proveedor"
                        << setw(20) << "Fecha Orden"
                        << setw(20) << "Fecha Ingreso"
                        << setw(30) << "Producto"
                        << setw(10) << "Cantidad"
                        << setw(15) << "Precio Costo" << endl;

                    cout << string(165, '-') << endl;

                    vector<tuple<string, int>> productos;
                    bool tiene_productos = false;

                    while ((fila = mysql_fetch_row(datos_compra))) {
                        string id = fila[0] ? fila[0] : "";
                        string no_orden = fila[1] ? fila[1] : "";
                        string proveedor = fila[2] ? fila[2] : "";
                        string fecha_orden = fila[3] ? fila[3] : "";
                        string fecha_ingreso = fila[4] ? fila[4] : "";
                        string nombre_producto = fila[5] ? fila[5] : "";
                        string cantidad_str = fila[6] ? fila[6] : "0";
                        string precio_str = fila[7] ? fila[7] : "0.00";

                        int cantidad = atoi(cantidad_str.c_str());

                        productos.push_back(make_tuple(nombre_producto, cantidad));
                        tiene_productos = true;

                        cout << left
                            << setw(10) << id
                            << setw(15) << no_orden
                            << setw(25) << proveedor
                            << setw(20) << fecha_orden
                            << setw(20) << fecha_ingreso
                            << setw(30) << nombre_producto
                            << setw(10) << cantidad_str
                            << setw(15) << precio_str << endl;
                    }

                    mysql_free_result(datos_compra);

                    if (!tiene_productos) {
                        cout << "No se encontraron productos asociados a la compra." << endl;
                        continue;
                    }

                    // Validar existencia de stock
                    bool stock_valido = true;
                    for (auto& prod : productos) {
                        string nombre_producto = get<0>(prod);
                        int cantidad = get<1>(prod);

                        string consulta_stock = "SELECT existencia FROM productos WHERE producto = '" + nombre_producto + "';";
                        if (mysql_query(cn.getConectar(), consulta_stock.c_str()) == 0) {
                            MYSQL_RES* res_stock = mysql_store_result(cn.getConectar());
                            MYSQL_ROW fila_stock = mysql_fetch_row(res_stock);

                            if (fila_stock) {
                                int existencia_actual = atoi(fila_stock[0]);
                                if (existencia_actual < cantidad) {
                                    cout << "\nNo se puede eliminar la compra. El producto " << nombre_producto
                                        << " tiene solo " << existencia_actual << " unidades en stock, y se requieren "
                                        << cantidad << " para restar." << endl;
                                    stock_valido = false;
                                    mysql_free_result(res_stock);
                                    break;
                                }
                            }
                            mysql_free_result(res_stock);
                        }
                    }

                    if (stock_valido) {
                        char confirmar;
                        cout << "\n¿Está seguro de eliminar esta compra y restar existencias? (s/n): ";
                        cin >> confirmar;
                        cin.ignore();

                        if (confirmar == 's' || confirmar == 'S') {
                            for (auto& prod : productos) {
                                string nombre_producto = get<0>(prod);
                                int cantidad = get<1>(prod);

                                string update_existencia = "UPDATE productos SET existencia = existencia - " + to_string(cantidad) +
                                    " WHERE producto = '" + nombre_producto + "';";
                                mysql_query(cn.getConectar(), update_existencia.c_str());
                            }

                            string eliminar_det = "DELETE FROM compras_detalle WHERE idCompra = " + to_string(idCompra) + ";";
                            string eliminar_compra = "DELETE FROM compras WHERE idCompra = " + to_string(idCompra) + ";";

                            if (mysql_query(cn.getConectar(), eliminar_det.c_str()) == 0 &&
                                mysql_query(cn.getConectar(), eliminar_compra.c_str()) == 0) {
                                cout << "Compra y existencias eliminadas exitosamente." << endl;
                            }
                            else {
                                cout << "Error al eliminar la compra o sus detalles." << endl;
                            }
                        }
                        else {
                            cout << "Eliminación cancelada." << endl;
                        }
                    }
                }
                else {
                    cout << "Error al consultar la compra." << endl;
                }
            }

            char repetir;
            cout << "\n¿Desea eliminar otra compra? (s/n): ";
            cin >> repetir;
            cin.ignore();
            if (repetir != 's' && repetir != 'S') break;
        }

        cn.cerrar_conexion();
    }

};
