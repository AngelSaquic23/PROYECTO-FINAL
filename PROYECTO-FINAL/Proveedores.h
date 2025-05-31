#pragma once

#include <string>
#include <iomanip>
#include "Conexionsql.h"

using namespace std;
class Proveedores {
private:
	string nit, direccion, proveedor;
	int telefono = 0;
public:
	Proveedores() {
	}
	Proveedores(string direc, string prov, int tel, string n) {
		nit = n;
		proveedor = prov;
		direccion = direc;
		telefono = tel;
	}
	void crear() const {
		int q_estado;
		conexion cn = conexion();
		cn.abrir_conexion();

		if (cn.getConectar()) {
			string t;
			string nit, dire, proveedor;

			cin.ignore();
			cout << "Ingrese NIT: ";
			getline(cin, nit);
			cout << "Ingrese Direccion: ";
			getline(cin, dire);
			cout << "Ingrese Proveedor: ";
			getline(cin, proveedor);
			cout << "Ingrese Telefono: ";
			getline(cin, t);

			string insert = "INSERT INTO proveedores(nit, direccion, proveedor, telefono) VALUES ('" + nit + "', '" + dire + "', '" + proveedor + "', '" + t + "')";
			const char* i = insert.c_str();

			q_estado = mysql_query(cn.getConectar(), i);
			if (!q_estado) {
				cout << "Ingreso Exitoso...." << endl;
			}
			else {
				cout << "----- Error al Ingresar -----" << endl;
			}
		}
		else {
			cout << "Error en la Conexion..." << endl;
		}
		cn.cerrar_conexion();
	}


	void leer() {
		int q_estado;
		conexion cn = conexion();
		MYSQL_ROW fila;
		MYSQL_RES* resultado;

		cn.abrir_conexion();
		if (cn.getConectar()) {
			string consulta = "SELECT * FROM proveedores";
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());

				cout << "\n----------------------------- PROVEEDORES -----------------------------" << endl;
				cout << left << setw(10) << "ID"
					<< setw(20) << "Proveedor"
					<< setw(15) << "NIT"
					<< setw(25) << "Dirección"
					<< setw(15) << "Teléfono" << endl;
				cout << string(85, '-') << endl;

				while ((fila = mysql_fetch_row(resultado))) {
					cout << left << setw(10) << fila[0]
						<< setw(20) << fila[1]
						<< setw(15) << fila[2]
						<< setw(25) << fila[3]
						<< setw(15) << fila[4] << endl;
				}
			}
			else {
				cout << "---- Error al Consultar ----" << endl;
			}
		}
		else {
			cout << "Error en la Conexión..." << endl;
		}
		cn.cerrar_conexion();
	}

	void actualizar() const {
		conexion cn{};
		char confirmar = 's';
		while (confirmar == 's' || confirmar == 'S') {
			cn.abrir_conexion();
			if (cn.getConectar()) {
				string id, proveedor, nit, direccion, telefono;
				cout << "Ingrese el ID del proveedor a modificar: ";
				cin >> id;
				cin.ignore();

				string consulta = "SELECT * FROM proveedores WHERE idProveedor = " + id;
				if (!mysql_query(cn.getConectar(), consulta.c_str())) {
					MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
					MYSQL_ROW fila = mysql_fetch_row(resultado);

					if (fila) {
						// Datos actuales
						string proveedor_actual = fila[1];
						string nit_actual = fila[2];
						string direccion_actual = fila[3];
						string telefono_actual = fila[4];

						cout << "\n------------- PROVEEDOR ENCONTRADO -------------\n";
						cout << left << setw(10) << "ID"
							<< setw(25) << "Proveedor"
							<< setw(15) << "NIT"
							<< setw(30) << "Direccion"
							<< setw(15) << "Telefono" << endl;
						cout << string(95, '-') << endl;
						cout << left << setw(10) << fila[0]
							<< setw(25) << fila[1]
							<< setw(15) << fila[2]
							<< setw(30) << fila[3]
							<< setw(15) << fila[4] << endl;

						cout << "\n¿Desea actualizar esta información? (s/n): ";
						char respuesta;
						cin >> respuesta;
						cin.ignore();

						if (respuesta == 's' || respuesta == 'S') {
							cout << "\nIngrese nuevo nombre del proveedor (Enter para conservar: " << proveedor_actual << "): ";
							getline(cin, proveedor);
							if (proveedor.empty()) proveedor = proveedor_actual;

							cout << "Ingrese nuevo NIT (Enter para conservar: " << nit_actual << "): ";
							getline(cin, nit);
							if (nit.empty()) nit = nit_actual;

							cout << "Ingrese nueva dirección (Enter para conservar: " << direccion_actual << "): ";
							getline(cin, direccion);
							if (direccion.empty()) direccion = direccion_actual;

							cout << "Ingrese nuevo teléfono (Enter para conservar: " << telefono_actual << "): ";
							getline(cin, telefono);
							if (telefono.empty()) telefono = telefono_actual;

							string update = "UPDATE proveedores SET proveedor = '" + proveedor +
								"', nit = '" + nit +
								"', direccion = '" + direccion +
								"', telefono = '" + telefono +
								"' WHERE idProveedor = " + id;

							if (!mysql_query(cn.getConectar(), update.c_str())) {
								cout << "\n>>> Proveedor actualizado exitosamente.\n";
							}
							else {
								cout << "\nXXX Error al actualizar el proveedor. XXX\n";
							}
						}
						else {
							cout << "Actualización cancelada.\n";
						}
					}
					else {
						cout << "No se encontró ningún proveedor con ese ID.\n";
					}
					mysql_free_result(resultado);
				}
				else {
					cout << "Error al consultar el proveedor.\n";
				}
				cn.cerrar_conexion();
			}
			else {
				cout << "Error en la conexión.\n";
			}

			cout << "\n¿Desea actualizar otro proveedor? (s/n): ";
			cin >> confirmar;
			cin.ignore();
		}
	}


	void eliminar() {
		conexion cn{};
		char confirmar = 's';

		while (confirmar == 's' || confirmar == 'S') {
			cn.abrir_conexion();
			if (cn.getConectar()) {
				string id;
				cout << "\nIngrese el ID del proveedor que desea eliminar: ";
				cin >> id;
				cin.ignore();

				string consulta = "SELECT * FROM proveedores WHERE idProveedor = " + id;
				if (!mysql_query(cn.getConectar(), consulta.c_str())) {
					MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
					MYSQL_ROW fila = mysql_fetch_row(resultado);

					if (fila) {
						// Mostrar datos actuales antes de eliminar
						cout << "\n------------- PROVEEDOR ENCONTRADO -------------\n";
						cout << left << setw(10) << "ID"
							<< setw(25) << "Proveedor"
							<< setw(15) << "NIT"
							<< setw(30) << "Direccion"
							<< setw(15) << "Telefono" << endl;
						cout << string(95, '-') << endl;
						cout << left << setw(10) << fila[0]
							<< setw(25) << fila[1]
							<< setw(15) << fila[2]
							<< setw(30) << fila[3]
							<< setw(15) << fila[4] << endl;

						cout << "\n¿Está seguro de que desea eliminar este proveedor? (s/n): ";
						char respuesta;
						cin >> respuesta;
						cin.ignore();

						if (respuesta == 's' || respuesta == 'S') {
							string eliminar = "DELETE FROM proveedores WHERE idProveedor = " + id;
							if (!mysql_query(cn.getConectar(), eliminar.c_str())) {
								cout << "\n>>> Registro eliminado exitosamente.\n";
							}
							else {
								cout << "\nXXX Error al eliminar el proveedor. XXX\n";
							}
						}
						else {
							cout << "Eliminación cancelada.\n";
						}
					}
					else {
						cout << "No se encontró ningún proveedor con ese ID.\n";
					}
					mysql_free_result(resultado);
				}
				else {
					cout << "Error al consultar el proveedor.\n";
				}
				cn.cerrar_conexion();
			}
			else {
				cout << "Error en la conexión.\n";
			}

			cout << "\n¿Desea eliminar otro proveedor? (s/n): ";
			cin >> confirmar;
			cin.ignore();
		}
	}

};
