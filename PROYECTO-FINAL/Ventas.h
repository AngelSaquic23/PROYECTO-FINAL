#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <mysql.h>
#include <cstdint>
#include <windows.h>
#include "gotoxy.h"
#include "conexionsql.h"

using namespace std;

class Ventas {
private:
    uint64_t idVenta = 0, idCliente = 0;
    string nit, nombreCliente, fecha, numeroFactura = "", serie = "A";
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
                numeroFactura = to_string(ultimo_num + 1);
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
            idCliente = 1; // Asegúrate de que exista un cliente con NIT "C/F" en tu tabla
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
                cout << "Cliente no encontrado. Ingrese nombres: ";
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

            cn.abrir_conexion();
            MYSQL* conectar = cn.getConectar();
            MYSQL_RES* resultado = nullptr;
            MYSQL_ROW fila;

            string consulta = "SELECT idProducto, producto, marca, precio_venta FROM productos WHERE producto LIKE '%" +
                cod + "%' OR idProducto = '" + cod + "' LIMIT 1";
            if (mysql_query(conectar, consulta.c_str()) == 0) {
                resultado = mysql_store_result(conectar);
                if (resultado && (fila = mysql_fetch_row(resultado))) {
                    idProducto.push_back(strtoull(fila[0], nullptr, 10));
                    producto.push_back(fila[1]);
                    marca.push_back(fila[2]);
                    double precio = atof(fila[3]);
                    precio_venta.push_back(precio);

                    int cant;
                    cout << "Cantidad: ";
                    cin >> cant;
                    cin.ignore();
                    cantidad.push_back(cant);
                    subtotal.push_back(cant * precio);
                    total += cant * precio;
                }
                else {
                    cout << "Producto no encontrado.\n";
                    --i;
                }
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
        cout << "\n================ FACTURA ================\n";
        cout << "Factura No: " << numeroFactura << " - Serie: " << serie << endl;
        cout << "Fecha: " << fecha << endl;
        cout << "Cliente: " << nombreCliente << "\tNIT: " << nit << endl;
        cout << "----------------------------------------\n";
        cout << left << setw(15) << "Producto" << setw(10) << "Marca" << setw(10) << "Cant"
            << setw(10) << "Precio" << setw(10) << "SubTotal" << endl;

        for (size_t i = 0; i < producto.size(); ++i) {
            cout << left << setw(15) << producto[i] << setw(10) << marca[i]
                << setw(10) << cantidad[i] << setw(10) << fixed << setprecision(2)
                << precio_venta[i] << setw(10) << subtotal[i] << endl;
        }

        cout << "----------------------------------------\n";
        cout << "Total: Q." << fixed << setprecision(2) << total << endl;
        cout << "========================================\n";
    }

public:
    void crear() {
        buscarCliente();
        fecha = generarFechaActual();
        obtenerNumeroFactura();
        ingresarProductos();
        guardarVenta();
        imprimirFacturaConsola();
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
            cout << left << setw(5) << "ID" << setw(10) << "Factura" << setw(8) << "Serie"
                << setw(12) << "Fecha" << setw(15) << "NIT" << setw(25) << "Cliente" << endl;
            cout << "---------------------------------------------------------------\n";

            while ((fila = mysql_fetch_row(resultado))) {
                cout << left << setw(5) << fila[0] << setw(10) << fila[1] << setw(8) << fila[2]
                    << setw(12) << fila[3] << setw(15) << (fila[4] ? fila[4] : "C/F") << setw(25)
                    << (fila[5] ? fila[5] : "Consumidor Final") << endl;
            }

            cout << "===============================================================\n";
            mysql_free_result(resultado);
        }
        else {
            cerr << "Error al ejecutar la consulta de ventas." << endl;
        }

        cn.cerrar_conexion();
    }
};
1234556778990