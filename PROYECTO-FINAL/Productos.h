#pragma once

#include "conexionsql.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <mysql.h>

using namespace std;

class Productos {
private:
    string producto, descripcion, fecha_ingreso, idMarca;
    double precio_costo = 0.0, precio_venta = 0.0;
    int existencia = 0;

public:
    // Constructor por defecto corregido
    Productos() {
        producto = "";
        descripcion = "";
        fecha_ingreso = "";
        idMarca = "";
        precio_costo = 0.0;
        precio_venta = 0.0;
        existencia = 0;
    }

    Productos(string prod, string desc, string fecha, string idM, double pc, double pv, int exist) {
        producto = prod;
        descripcion = desc;
        fecha_ingreso = fecha;
        idMarca = idM;
        precio_costo = pc;
        precio_venta = pv;
        existencia = exist;
    }

    void crear() {
        conexion cn{};
        cn.abrir_conexion();

        if (cn.getConectar()) {
            cout << "Nombre del producto: ";
            cin.ignore();
            getline(cin, producto);

            cout << "Descripcion: ";
            getline(cin, descripcion);

            cout << "Precio costo: ";
            cin >> precio_costo;

            cout << "Precio venta: ";
            cin >> precio_venta;

            cout << "Existencia: ";
            cin >> existencia;
            cin.ignore();

            cout << "ID de la marca: ";
            getline(cin, idMarca);

            cout << "Fecha de ingreso (YYYY-MM-DD): ";
            getline(cin, fecha_ingreso);

            string insert = "INSERT INTO productos(producto, descripcion, precio_costo, precio_venta, existencia, idMarca, fecha_ingreso) VALUES('" +
                producto + "', '" + descripcion + "', " + to_string(precio_costo) + ", " + to_string(precio_venta) + ", " + to_string(existencia) +
                ", '" + idMarca + "', '" + fecha_ingreso + "')";

            if (!mysql_query(cn.getConectar(), insert.c_str())) {
                cout << "Producto ingresado exitosamente.\n";
            }
            else {
                cout << "Error al ingresar producto.\n";
                cout << "Consulta: " << insert << endl;
            }
        }
        else {
            cout << "Error de conexión.\n";
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
        conexion cn;
        cn.abrir_conexion();

        if (cn.getConectar()) {
            string idProducto;
            cout << "Ingrese el ID del producto a actualizar: ";
            cin >> idProducto;
            cin.ignore();

            string consulta = "SELECT producto, descripcion, precio_costo, precio_venta, existencia, idMarca, DATE(fecha_ingreso) AS fecha_ingreso FROM productos WHERE idProducto = " + idProducto;

            if (!mysql_query(cn.getConectar(), consulta.c_str())) {
                MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
                MYSQL_ROW fila = mysql_fetch_row(resultado);

                if (fila) {
                    string prod_actual = fila[0];
                    string desc_actual = fila[1];
                    string pc_actual = fila[2];
                    string pv_actual = fila[3];
                    string stock_actual = fila[4];
                    string marca_actual = fila[5];
                    string fecha_actual = fila[6];

                    cout << "\n---------- DATOS ACTUALES DEL PRODUCTO ----------\n";
                    cout << left << setw(20) << "Producto"
                        << setw(25) << "Descripcion"
                        << setw(12) << "P. Costo"
                        << setw(12) << "P. Venta"
                        << setw(10) << "Stock"
                        << setw(10) << "Marca"
                        << setw(15) << "Ingreso" << endl;
                    cout << string(110, '-') << endl;
                    cout << left << setw(20) << prod_actual
                        << setw(25) << desc_actual
                        << setw(12) << pc_actual
                        << setw(12) << pv_actual
                        << setw(10) << stock_actual
                        << setw(10) << marca_actual
                        << setw(15) << fecha_actual << endl;

                    char opcion;
                    cout << "\nDesea actualizar este producto (s/n): ";
                    cin >> opcion;
                    cin.ignore();

                    if (opcion == 's' || opcion == 'S') {
                        string prod, desc, pc, pv, stock, marca, fecha;

                        cout << "Nuevo producto (Enter para conservar): ";
                        getline(cin, prod);
                        if (prod.empty()) prod = prod_actual;

                        cout << "Nueva descripcion: ";
                        getline(cin, desc);
                        if (desc.empty()) desc = desc_actual;

                        cout << "Nuevo precio costo: ";
                        getline(cin, pc);
                        if (pc.empty()) pc = pc_actual;

                        cout << "Nuevo precio venta: ";
                        getline(cin, pv);
                        if (pv.empty()) pv = pv_actual;

                        cout << "Nueva existencia: ";
                        getline(cin, stock);
                        if (stock.empty()) stock = stock_actual;

                        cout << "Nuevo ID de marca: ";
                        getline(cin, marca);
                        if (marca.empty()) marca = marca_actual;

                        cout << "Nueva fecha de ingreso: ";
                        getline(cin, fecha);
                        if (fecha.empty()) fecha = fecha_actual;

                        string update = "UPDATE productos SET producto = '" + prod +
                            "', descripcion = '" + desc +
                            "', precio_costo = " + pc +
                            ", precio_venta = " + pv +
                            ", existencia = " + stock +
                            ", idMarca = '" + marca +
                            "', fecha_ingreso = '" + fecha +
                            "' WHERE idProducto = " + idProducto;

                        if (!mysql_query(cn.getConectar(), update.c_str())) {
                            cout << "\nProducto actualizado exitosamente.\n";
                        }
                        else {
                            cout << "Error al actualizar producto.\n";
                            cout << "Consulta: " << update << endl;
                        }
                    }
                }
                else {
                    cout << "Producto no encontrado.\n";
                }

                mysql_free_result(resultado);
            }
            else {
                cout << "Error al consultar producto.\n";
            }
        }
        else {
            cout << "Error de conexion.\n";
        }

        cn.cerrar_conexion();
    }

    void eliminar() {
        conexion cn;
        cn.abrir_conexion();

        if (cn.getConectar()) {
            string idProducto;
            cout << "Ingrese el ID del producto a eliminar: ";
            cin >> idProducto;
            cin.ignore();

            string consulta = "SELECT p.idProducto, p.producto, p.descripcion, p.precio_costo, p.precio_venta, p.existencia, m.marca, DATE(p.fecha_ingreso) FROM productos p INNER JOIN marcas m ON p.idMarca = m.idMarca WHERE p.idProducto = " + idProducto;

            if (!mysql_query(cn.getConectar(), consulta.c_str())) {
                MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
                MYSQL_ROW fila = mysql_fetch_row(resultado);

                if (fila) {
                    cout << "\n---------- DATOS DEL PRODUCTO A ELIMINAR ----------\n";
                    cout << left << setw(5) << "ID"
                        << setw(20) << "Producto"
                        << setw(30) << "Descripcion"
                        << setw(12) << "P. Costo"
                        << setw(12) << "P. Venta"
                        << setw(10) << "Stock"
                        << setw(20) << "Marca"
                        << setw(15) << "Ingreso" << endl;
                    cout << string(124, '-') << endl;

                    cout << left << setw(5) << fila[0]
                        << setw(20) << fila[1]
                        << setw(30) << fila[2]
                        << setw(12) << fila[3]
                        << setw(12) << fila[4]
                        << setw(10) << fila[5]
                        << setw(20) << fila[6]
                        << setw(15) << fila[7] << endl;

                    char confirmar;
                    cout << "\nDesea eliminar este producto? (s/n): ";
                    cin >> confirmar;

                    if (confirmar == 's' || confirmar == 'S') {
                        string eliminar = "DELETE FROM productos WHERE idProducto = " + idProducto;

                        if (!mysql_query(cn.getConectar(), eliminar.c_str())) {
                            cout << "Producto eliminado correctamente.\n";
                        }
                        else {
                            cout << "Error al eliminar producto.\n";
                        }
                    }
                    else {
                        cout << "Operacion cancelada.\n";
                    }

                    mysql_free_result(resultado);
                }
                else {
                    cout << "Producto no encontrado.\n";
                }
            }
            else {
                cout << "Error al consultar producto.\n";
            }
        }
        else {
            cout << "Error de conexion.\n";
        }

        cn.cerrar_conexion();
    }


};
