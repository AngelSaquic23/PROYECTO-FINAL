#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <sstream>
#include <mysql.h>
#include <algorithm>
#include <istream>
#include <cstdint>
#include <windows.h>
#include "gotoxy.h"
#include "conexionsql.h"

using namespace std;

class Ventas {
private:
    uint64_t idVenta = 0, idCliente = 0;
    string nit, nombreCliente, fecha, numeroFactura = "", serie = "";
    double total = 0.0;

    vector<uint64_t> idProducto;
    vector<string> producto, marca;
    vector<double> precio_venta, subtotal;
    vector<int> cantidad;

    conexion cn;

    string generarFechaActual() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char fecha_actual[11];
        snprintf(fecha_actual, sizeof(fecha_actual), "%04d-%02d-%02d",
            1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
        return string(fecha_actual);
    }

    void obtenerNumeroFactura() {
        cn.abrir_conexion();
        MYSQL* conectar = cn.getConectar();
        MYSQL_RES* resultado = nullptr;
        MYSQL_ROW fila;

        string consulta = "SELECT IFNULL(MAX(nofactura), 0) FROM ventas";
        if (mysql_query(conectar, consulta.c_str()) == 0) {
            resultado = mysql_store_result(conectar);
            if (resultado && (fila = mysql_fetch_row(resultado))) {
                int ultimo_num = atoi(fila[0]);
                int nuevo_num = ultimo_num + 1;

                // Generar número de factura
                numeroFactura = to_string(nuevo_num);

                // Generar serie con formato V00X
                stringstream ss;
                ss << "V" << setfill('0') << setw(3) << nuevo_num;
                serie = ss.str();
            }
        }
        else {
            cerr << "Error al obtener número de factura.\n";
        }

        if (resultado) mysql_free_result(resultado);
        cn.cerrar_conexion();
    }

    void buscarCliente() {
        cout << "Ingrese NIT (C/F para consumidor final): ";
        getline(cin, nit);
        for (char& c : nit) c = toupper(c);

        if (nit == "CF" || nit == "C/F") {
            nit = "C/F";
            nombreCliente = "Consumidor Final";
            idCliente = 1;
            return;
        }

        cn.abrir_conexion();
        MYSQL* conectar = cn.getConectar();
        MYSQL_RES* resultado = nullptr;
        MYSQL_ROW fila;

        string consulta = "SELECT idCliente, nombres, apellidos FROM clientes WHERE NIT = '" + nit + "'";
        if (mysql_query(conectar, consulta.c_str()) == 0) {
            resultado = mysql_store_result(conectar);
            if (resultado && (fila = mysql_fetch_row(resultado))) {
                idCliente = strtoull(fila[0], nullptr, 10);
                nombreCliente = string(fila[1]) + " " + string(fila[2]);
            }
            else {
                cout << "Cliente no encontrado.\nIngrese nombres: ";
                string nombres, apellidos;
                cout << "Nombres: "; getline(cin, nombres);
                cout << "Apellidos: "; getline(cin, apellidos);

                string insert = "INSERT INTO clientes(nombres, apellidos, NIT, fechaingreso) VALUES ('" +
                    nombres + "', '" + apellidos + "', '" + nit + "', NOW())";
                if (mysql_query(conectar, insert.c_str()) == 0) {
                    idCliente = mysql_insert_id(conectar);
                    nombreCliente = nombres + " " + apellidos;
                }
                else {
                    cerr << "Error al registrar cliente.\n";
                }
            }
        }

        if (resultado) mysql_free_result(resultado);
        cn.cerrar_conexion();
    }

    void ingresarProductos() {
        int n;
        cout << "Cantidad de productos a ingresar: ";
        cin >> n;
        cin.ignore();

        for (int i = 0; i < n; ++i) {
            string cod;
            cout << "\nProducto " << i + 1 << ": Ingrese ID o nombre: ";
            getline(cin, cod);

            bool esNumero = all_of(cod.begin(), cod.end(), ::isdigit);

            cn.abrir_conexion();
            MYSQL* conectar = cn.getConectar();
            MYSQL_RES* resultado = nullptr;
            MYSQL_ROW fila;

            string consulta;
            if (esNumero) {
                consulta =
                    "SELECT p.idProducto, p.producto, m.marca, p.precio_venta "
                    "FROM productos p "
                    "INNER JOIN marcas m ON p.idMarca = m.idMarca "
                    "WHERE p.idProducto = " + cod + " LIMIT 1";
            }
            else {
                consulta =
                    "SELECT p.idProducto, p.producto, m.marca, p.precio_venta "
                    "FROM productos p "
                    "INNER JOIN marcas m ON p.idMarca = m.idMarca "
                    "WHERE p.producto LIKE '%" + cod + "%' LIMIT 1";
            }

            if (mysql_query(conectar, consulta.c_str()) == 0) {
                resultado = mysql_store_result(conectar);
                if (resultado && (fila = mysql_fetch_row(resultado))) {
                    int cant;
                    cout << "Cantidad: ";
                    cin >> cant;
                    cin.ignore();

                    idProducto.push_back(strtoull(fila[0], nullptr, 10));
                    producto.push_back(fila[1]);
                    marca.push_back(fila[2]);
                    double precio = atof(fila[3]);
                    precio_venta.push_back(precio);
                    cantidad.push_back(cant);
                    subtotal.push_back(cant * precio);
                    total += cant * precio;
                }
                else {
                    cout << "Producto no encontrado.\n";
                    --i;
                }
            }
            else {
                cerr << "Error al consultar el producto.\n";
                --i;
            }

            if (resultado) mysql_free_result(resultado);
            cn.cerrar_conexion();
        }
    }

    void guardarVenta() {
        cn.abrir_conexion();
        MYSQL* conectar = cn.getConectar();

        string insert_venta = "INSERT INTO ventas(nofactura, serie, fechafactura, idCliente, idEmpleado, fechaIngreso) VALUES (" +
            numeroFactura + ", '" + serie + "', '" + fecha + "', " + to_string(idCliente) + ", 1, NOW())";
        if (mysql_query(conectar, insert_venta.c_str()) == 0) {
            idVenta = mysql_insert_id(conectar);

            for (size_t i = 0; i < idProducto.size(); ++i) {
                string insert_det = "INSERT INTO ventas_detalle(idVenta, idProducto, cantidad, precio_unitario) VALUES (" +
                    to_string(idVenta) + ", " + to_string(idProducto[i]) + ", " +
                    to_string(cantidad[i]) + ", " + to_string(precio_venta[i]) + ")";
                mysql_query(conectar, insert_det.c_str());
            }

            cout << "\n? Venta registrada correctamente.\n";
        }
        else {
            cerr << "? Error al insertar la venta.\n";
        }

        cn.cerrar_conexion();
    }

    void imprimirFacturaConsola() {
        cout << "\n======================== FACTURA ==========================\n";
        cout << "\n                DIGITAL SOLUTIONS S.A \n";
        cout << "Factura No: " << numeroFactura << " - Serie: " << serie << endl;
        cout << "Fecha: " << fecha << endl;
        cout << "Cliente: " << nombreCliente << "       \tNIT: " << nit << endl;
        cout << "------------------------------------------------------------\n";
        cout << left << setw(15) << "Producto" << setw(10) << "Marca" << setw(10) << "Cant"
            << setw(10) << "Precio" << setw(10) << "SubTotal" << endl;

        for (size_t i = 0; i < producto.size(); ++i) {
            cout << left << setw(15) << producto[i] << setw(10) << marca[i]
                << setw(10) << cantidad[i] << setw(10) << fixed << setprecision(2)
                << precio_venta[i] << setw(10) << subtotal[i] << endl;
        }

        cout << "------------------------------------------------------------\n";
        cout << "                                    Total: Q." << fixed << setprecision(2) << total << endl;
        cout << "============================================================\n";
        cout << "          Gracias por su compra Vuelva Pronto!\n";
    }

    void generarFacturaArchivo() {
        // Crear nombre de archivo con formato Fact0001.txt
        stringstream nombre_archivo;
        nombre_archivo << "C:\\Users\\asaquic\\source\\repos\\PROYECTO-FINAL\\PROYECTO-FINAL\\Facturas\\Fact"
            << setfill('0') << setw(4) << numeroFactura << ".txt";

        // Abrir archivo
        ofstream archivo(nombre_archivo.str());
        if (!archivo.is_open()) {
            cerr << " No se pudo crear el archivo de factura.\n";
            return;
        }

        archivo << "\n======================== FACTURA ==========================\n";
        archivo << "Factura No: " << numeroFactura << " - Serie: " << serie << endl;
        archivo << "Fecha: " << fecha << endl;
        archivo << "Cliente: " << nombreCliente << "       \tNIT: " << nit << endl;
        archivo << "-----------------------------------------------------------\n";
        archivo << left << setw(15) << "Producto" << setw(10) << "Marca" << setw(10) << "Cant"
            << setw(10) << "Precio" << setw(10) << "SubTotal" << endl;

        for (size_t i = 0; i < producto.size(); ++i) {
            archivo << left << setw(15) << producto[i] << setw(10) << marca[i]
                << setw(10) << cantidad[i] << setw(10) << fixed << setprecision(2)
                << precio_venta[i] << setw(10) << subtotal[i] << endl;
        }

        archivo << "------------------------------------------------------------\n";
        archivo << "                                    Total: Q." << fixed << setprecision(2) << total << endl;
        archivo << "============================================================\n";
        archivo << "          Gracias por su compra Vuelva Pronto! ";
        archivo.close();

        // Abrir archivo automáticamente después de crearlo
        system(string("start " + nombre_archivo.str()).c_str());
    }

    void imprimirFacturaDesdeBD(string noFactura) {
        cn.abrir_conexion();
        MYSQL* conectar = cn.getConectar();
        MYSQL_RES* resultado = nullptr;
        MYSQL_ROW fila;

        // Datos generales
        string consulta =
            "SELECT v.idVenta, v.nofactura, v.serie, v.fechafactura, c.nit, "
            "CONCAT(c.nombres, ' ', c.apellidos) AS cliente "
            "FROM ventas v "
            "LEFT JOIN clientes c ON v.idCliente = c.idCliente "
            "WHERE v.nofactura = " + noFactura + " LIMIT 1";

        if (mysql_query(conectar, consulta.c_str()) == 0) {
            resultado = mysql_store_result(conectar);
            if ((fila = mysql_fetch_row(resultado))) {
                // Cargar datos
                idVenta = strtoull(fila[0], nullptr, 10);
                numeroFactura = fila[1];
                serie = fila[2];
                fecha = fila[3];
                nit = fila[4] ? fila[4] : "C/F";
                nombreCliente = fila[5] ? fila[5] : "Consumidor Final";
            }
            else {
                cout << "\n? No se encontró la factura.\n";
                cn.cerrar_conexion();
                return;
            }
            mysql_free_result(resultado);
        }
        else {
            cerr << "? Error al buscar datos de factura.\n";
            cn.cerrar_conexion();
            return;
        }

        // Detalles
        string consulta_detalle =
            "SELECT p.producto, m.marca, d.cantidad, d.precio_unitario "
            "FROM ventas_detalle d "
            "INNER JOIN productos p ON d.idProducto = p.idProducto "
            "INNER JOIN marcas m ON p.idMarca = m.idMarca "
            "WHERE d.idVenta = " + to_string(idVenta);

        if (mysql_query(conectar, consulta_detalle.c_str()) == 0) {
            resultado = mysql_store_result(conectar);

            // Limpiar vectores
            producto.clear();
            marca.clear();
            cantidad.clear();
            precio_venta.clear();
            subtotal.clear();
            total = 0.0;

            while ((fila = mysql_fetch_row(resultado))) {
                producto.push_back(fila[0]);
                marca.push_back(fila[1]);
                cantidad.push_back(atoi(fila[2]));
                double precio = atof(fila[3]);
                precio_venta.push_back(precio);
                subtotal.push_back(precio * atoi(fila[2]));
                total += precio * atoi(fila[2]);
            }
            mysql_free_result(resultado);
        }
        else {
            cerr << " Error al obtener detalles de la factura.\n";
            cn.cerrar_conexion();
            return;
        }

        cn.cerrar_conexion();

        // Llama al generador de archivo
        generarFacturaArchivo();
    }

public:
    void crear() {
        // Limpiar datos previos
        idVenta = 0;
        idCliente = 0;
        nit.clear();
        nombreCliente.clear();
        fecha.clear();
        numeroFactura.clear();
        total = 0.0;

        idProducto.clear();
        producto.clear();
        marca.clear();
        precio_venta.clear();
        subtotal.clear();
        cantidad.clear();

        cin.ignore();
        buscarCliente();
        fecha = generarFechaActual();
        obtenerNumeroFactura();
        ingresarProductos();
        guardarVenta();
        imprimirFacturaConsola();
        generarFacturaArchivo(); // NUEVO

    }

    void mostrar() {
        cn.abrir_conexion();
        MYSQL* conectar = cn.getConectar();
        MYSQL_RES* resultado;
        MYSQL_ROW fila;

        string consulta =
            "SELECT v.idVenta, v.nofactura, v.serie, v.fechafactura, c.nit, "
            "CONCAT(c.nombres, ' ', c.apellidos) AS cliente "
            "FROM ventas v "
            "LEFT JOIN clientes c ON v.idCliente = c.idCliente "
            "ORDER BY v.idVenta DESC";

        if (!mysql_query(conectar, consulta.c_str())) {
            resultado = mysql_store_result(conectar);
            cout << "\n================ LISTADO DE VENTAS ================\n";
            cout << left << setw(5) << "ID"
                << setw(10) << "Factura"
                << setw(8) << "Serie"
                << setw(12) << "Fecha"
                << setw(15) << "NIT"
                << setw(25) << "Cliente" << endl;
            cout << "---------------------------------------------------------------\n";

            while ((fila = mysql_fetch_row(resultado))) {
                cout << left << setw(5) << fila[0]
                    << setw(10) << fila[1]
                    << setw(8) << fila[2]
                    << setw(12) << fila[3]
                    << setw(15) << (fila[4] ? fila[4] : "C/F")
                    << setw(25) << (fila[5] ? fila[5] : "Consumidor Final") << endl;
            }

            cout << "===============================================================\n";
            mysql_free_result(resultado);
        }
        else {
            cerr << "Error al ejecutar la consulta de ventas." << endl;
        }

        char opcion;
        cout << "\nQue deseas hacer?\n";
        cout << "1. Imprimir factura\n";
        cout << "3. Salir al menu principal\n";
        cout << "Elige una opción: ";
        cin >> opcion;
        cin.ignore();

        if (opcion == '1') {
            string numFact;
            cout << "Ingrese el numero de factura que desea imprimir: ";
            getline(cin, numFact);
            imprimirFacturaDesdeBD(numFact);
        }
        else {
            cout << "\nRegresando al menu...\n";
        }
        cn.cerrar_conexion();
    }

    void actualizar() {
        string nit_o_factura, opcion;
        bool continuar = true;

        while (continuar) {
            cout << "\nIngrese el NIT del cliente o el número de factura: ";
            cin >> nit_o_factura;

            // Buscar datos generales de la factura
            string consulta =
                "SELECT v.idVenta, v.nofactura, v.serie, v.fechafactura, v.idCliente, "
                "CONCAT(c.nombres, ' ', c.apellidos) AS cliente "
                "FROM ventas v "
                "LEFT JOIN clientes c ON v.idCliente = c.idCliente "
                "WHERE c.nit = '" + nit_o_factura + "' OR v.nofactura = '" + nit_o_factura + "'";

            cn.abrir_conexion();
            MYSQL* conectar = cn.getConectar();
            MYSQL_RES* resultado;
            MYSQL_ROW fila;

            if (!mysql_query(conectar, consulta.c_str())) {
                resultado = mysql_store_result(conectar);
                if ((fila = mysql_fetch_row(resultado))) {
                    string idVenta = fila[0];
                    string nofactura = fila[1];
                    string idCliente = fila[4];

                    cout << "\n? Factura encontrada:\n";
                    cout << "Factura No.: " << fila[1] << " | Serie: " << fila[2] << " | Fecha: " << fila[3] << endl;
                    cout << "Cliente: " << fila[5] << endl;

                    // Mostrar productos actuales
                    string consulta_detalle =
                        "SELECT vd.idProducto, p.producto, vd.cantidad, vd.precio_unitario "
                        "FROM ventas_detalle vd "
                        "JOIN productos p ON vd.idProducto = p.idProducto "
                        "WHERE vd.idVenta = " + idVenta;

                    if (!mysql_query(conectar, consulta_detalle.c_str())) {
                        MYSQL_RES* resultado_detalle = mysql_store_result(conectar);
                        MYSQL_ROW fila_detalle;

                        cout << "\nProductos actuales en la venta:\n";
                        cout << left << setw(10) << "ID" << setw(25) << "Producto" << setw(10) << "Cant" << setw(15) << "Precio Unitario" << endl;
                        cout << "---------------------------------------------------------------\n";
                        while ((fila_detalle = mysql_fetch_row(resultado_detalle))) {
                            cout << left << setw(10) << fila_detalle[0] << setw(25) << fila_detalle[1]
                                << setw(10) << fila_detalle[2] << setw(15) << fila_detalle[3] << endl;
                        }
                        mysql_free_result(resultado_detalle);
                    }

                    cout << "\n¿Desea actualizar esta factura? (s/n): ";
                    cin >> opcion;

                    if (opcion == "s" || opcion == "S") {
                        // Borrar detalles anteriores
                        string borrar_detalles = "DELETE FROM ventas_detalle WHERE idVenta = " + idVenta;
                        mysql_query(conectar, borrar_detalles.c_str());

                        char agregar = 's';
                        while (agregar == 's' || agregar == 'S') {
                            string idProducto;
                            int cantidad = -1;
                            double precio = 0.0;

                            cout << "\nIngrese ID del producto: ";
                            cin >> idProducto;

                            while (cantidad < 0) {
                                cout << "Cantidad (0 para omitir): ";
                                cin >> cantidad;
                            }

                            if (cantidad == 0) {
                                cout << "? Producto omitido.\n";
                            }
                            else {
                                // Obtener precio
                                string consulta_precio = "SELECT precio_venta FROM productos WHERE idProducto = " + idProducto;
                                if (!mysql_query(conectar, consulta_precio.c_str())) {
                                    MYSQL_RES* r = mysql_store_result(conectar);
                                    MYSQL_ROW f = mysql_fetch_row(r);
                                    if (f) {
                                        precio = atof(f[0]);
                                    }
                                    mysql_free_result(r);
                                }

                                // Insertar nuevo detalle
                                string insertar = "INSERT INTO ventas_detalle (idVenta, idProducto, cantidad, precio_unitario) VALUES (" +
                                    idVenta + ", " + idProducto + ", " + to_string(cantidad) + ", " + to_string(precio) + ")";
                                mysql_query(conectar, insertar.c_str());
                            }

                            cout << "¿Desea agregar otro producto? (s/n): ";
                            cin >> agregar;
                        }

                        cout << "\n? Factura actualizada con éxito.\n";
                        cout << " Generando nueva factura...\n";
                        imprimirFacturaDesdeBD(nofactura); // Aquí se usa tu función actual
                    }
                    else {
                        cout << "?? Actualización cancelada.\n";
                    }

                    cout << "\n¿Desea actualizar otra factura? (s/n): ";
                    cin >> opcion;
                    if (opcion == "n" || opcion == "N") continuar = false;
                }
                else {
                    cout << "? No se encontró la factura.\n";
                    cout << "¿Desea intentar con otra? (s/n): ";
                    cin >> opcion;
                    if (opcion == "n" || opcion == "N") continuar = false;
                }

                mysql_free_result(resultado);
            }
            else {
                cerr << "? Error al ejecutar la consulta.\n";
                continuar = false;
            }

            cn.cerrar_conexion();
        }
    }
    void eliminar() {
        string nit_o_factura, opcion;
        bool continuar = true;

        while (continuar) {
            cout << "\nIngrese el NIT del cliente o el numero de factura: ";
            cin >> nit_o_factura;

            string consulta =
                "SELECT v.idVenta, v.nofactura, v.serie, v.fechafactura, v.idCliente, "
                "CONCAT(c.nombres, ' ', c.apellidos) AS cliente "
                "FROM ventas v "
                "LEFT JOIN clientes c ON v.idCliente = c.idCliente "
                "WHERE c.nit = '" + nit_o_factura + "' OR v.nofactura = '" + nit_o_factura + "'";

            cn.abrir_conexion();
            MYSQL* conectar = cn.getConectar();
            MYSQL_RES* resultado;
            MYSQL_ROW fila;

            if (!mysql_query(conectar, consulta.c_str())) {
                resultado = mysql_store_result(conectar);
                if ((fila = mysql_fetch_row(resultado))) {
                    string idVenta = fila[0];
                    string nofactura = fila[1];

                    cout << "\nFactura encontrada:\n";
                    cout << "Factura No.: " << fila[1] << " | Serie: " << fila[2] << " | Fecha: " << fila[3] << endl;
                    cout << "Cliente: " << fila[5] << endl;

                    string consulta_detalle =
                        "SELECT vd.idProducto, p.producto, vd.cantidad, vd.precio_unitario "
                        "FROM ventas_detalle vd "
                        "JOIN productos p ON vd.idProducto = p.idProducto "
                        "WHERE vd.idVenta = " + idVenta;

                    if (!mysql_query(conectar, consulta_detalle.c_str())) {
                        MYSQL_RES* resultado_detalle = mysql_store_result(conectar);
                        MYSQL_ROW fila_detalle;

                        cout << "\nProductos actuales en la venta:\n";
                        cout << left << setw(10) << "ID" << setw(25) << "Producto" << setw(10) << "Cant" << setw(15) << "Precio Unitario" << endl;
                        cout << "---------------------------------------------------------------\n";
                        while ((fila_detalle = mysql_fetch_row(resultado_detalle))) {
                            cout << left << setw(10) << fila_detalle[0] << setw(25) << fila_detalle[1]
                                << setw(10) << fila_detalle[2] << setw(15) << fila_detalle[3] << endl;
                        }
                        mysql_free_result(resultado_detalle);
                    }

                    cout << "\nÂ¿Desea eliminar esta factura? (s/n): ";
                    cin >> opcion;

                    if (opcion == "s" || opcion == "S") {
                        string borrar_detalles = "DELETE FROM ventas_detalle WHERE idVenta = " + idVenta;
                        string borrar_factura = "DELETE FROM ventas WHERE idVenta = " + idVenta;

                        if (mysql_query(conectar, borrar_detalles.c_str()) == 0 &&
                            mysql_query(conectar, borrar_factura.c_str()) == 0) {
                            cout << "\nâœ… Factura eliminada correctamente.\n";
                        }
                        else {
                            cerr << "\nâŒ Error al eliminar la factura.\n";
                        }
                    }
                    else {
                        cout << "â• Eliminacion cancelada.\n";
                    }

                    cout << "\nÂ¿Desea eliminar otra factura? (s/n): ";
                    cin >> opcion;
                    if (opcion == "n" || opcion == "N") continuar = false;
                }
                else {
                    cout << "âŒ No se encontro la factura.\n";
                    cout << "Â¿Desea intentar con otra? (s/n): ";
                    cin >> opcion;
                    if (opcion == "n" || opcion == "N") continuar = false;
                }

                mysql_free_result(resultado);
            }
            else {
                cerr << "âŒ Error al ejecutar la consulta.\n";
                continuar = false;
            }

            cn.cerrar_conexion();
        }
    }
};
