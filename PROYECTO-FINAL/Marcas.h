#pragma once

#include <string>
#include <iomanip>
#include "Conexionsql.h"

using namespace std;
class Marcas {

private:
	string marcas;

public:
	Marcas() {
	}
	Marcas(string marc) {
		marcas = marc;
	}
	void crear() {
		int q_estado;
		conexion cn = conexion();
		cn.abrir_conexion();

		if (cn.getConectar()) {
			string marca;
			cout << "Ingresar nombre de la marca: ";
			cin.ignore();
			getline(cin, marca);

			string insert = "INSERT INTO marcas(marca) VALUES ('" + marca + "')";
			const char* i = insert.c_str();
			q_estado = mysql_query(cn.getConectar(), i);

			if (!q_estado) {
				cout << "Marca ingresada exitosamente.\n";
			}
			else {
				cout << "Error al ingresar la marca.\n";
			}
		}
		else {
			cout << "Error en la conexion.\n";
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
			string consulta = "SELECT * FROM marcas";
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());

				cout << "\n------------- LISTADO DE MARCAS -------------\n";
				cout << left << setw(10) << "ID" << setw(30) << "Marca" << endl;
				cout << string(40, '-') << endl;

				while ((fila = mysql_fetch_row(resultado))) {
					cout << left << setw(10) << fila[0] << setw(30) << fila[1] << endl;
				}

				mysql_free_result(resultado);
				cout << string(40, '-') << endl;
			}
			else {
				cout << "Error en la consulta.\n";
			}
		}
		else {
			cout << "Error en la conexion.\n";
		}
		cn.cerrar_conexion();
	}

	void actualizar() {
		conexion cn{};
		string id, nueva_marca;
		char confirmar;

		do {
			cn.abrir_conexion();
			if (cn.getConectar()) {
				cout << "Ingrese el ID de la marca a modificar: ";
				cin >> id;
				cin.ignore();

				string consulta = "SELECT * FROM marcas WHERE idMarca = " + id;
				if (!mysql_query(cn.getConectar(), consulta.c_str())) {
					MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
					MYSQL_ROW fila = mysql_fetch_row(resultado);

					if (fila) {
						string marca_actual = fila[1];

						cout << "\n----------- MARCA ENCONTRADA -----------\n";
						cout << left << setw(10) << "ID" << setw(30) << "Marca" << endl;
						cout << string(40, '-') << endl;
						cout << left << setw(10) << fila[0] << setw(30) << fila[1] << endl;

						cout << "\n¿Desea actualizar esta marca? (s/n): ";
						cin >> confirmar;
						cin.ignore();

						if (confirmar == 's' || confirmar == 'S') {
							cout << "Ingrese el nuevo nombre de la marca (Enter para conservar: " << marca_actual << "): ";
							getline(cin, nueva_marca);
							if (nueva_marca.empty()) nueva_marca = marca_actual;

							string update = "UPDATE marcas SET marca = '" + nueva_marca + "' WHERE idMarca = " + id;
							if (!mysql_query(cn.getConectar(), update.c_str())) {
								cout << ">>> Marca actualizada correctamente.\n";
							}
							else {
								cout << "XXX Error al actualizar la marca. XXX\n";
							}

							cout << "\n¿Desea actualizar otra marca? (s/n): ";
							cin >> confirmar;
							cin.ignore();
						}
						else {
							cout << "Actualización cancelada.\n";
							cout << "\n¿Desea buscar otra marca? (s/n): ";
							cin >> confirmar;
							cin.ignore();
						}
					}
					else {
						cout << "No se encontró ninguna marca con ese ID.\n";
						cout << "\n¿Desea intentar con otro ID? (s/n): ";
						cin >> confirmar;
						cin.ignore();
					}
					mysql_free_result(resultado);
				}
				else {
					cout << "Error al consultar la marca.\n";
					confirmar = 'n';
				}
			}
			else {
				cout << "Error en la conexion.\n";
				confirmar = 'n';
			}
			cn.cerrar_conexion();
		} while (confirmar == 's' || confirmar == 'S');
	}

	void eliminar() {
		conexion cn{};
		string id;
		char confirmar;

		do {
			cn.abrir_conexion();
			if (cn.getConectar()) {
				cout << "Ingrese el ID de la marca a eliminar: ";
				cin >> id;

				string consulta = "SELECT * FROM marcas WHERE idMarca = " + id;
				if (!mysql_query(cn.getConectar(), consulta.c_str())) {
					MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
					MYSQL_ROW fila = mysql_fetch_row(resultado);

					if (fila) {
						cout << "\n----------- MARCA A ELIMINAR -----------\n";
						cout << left << setw(10) << "ID" << setw(30) << "Marca" << endl;
						cout << string(40, '-') << endl;
						cout << left << setw(10) << fila[0] << setw(30) << fila[1] << endl;

						cout << "\n¿Desea eliminar esta marca? (s/n): ";
						cin >> confirmar;

						if (confirmar == 's' || confirmar == 'S') {
							string eliminar = "DELETE FROM marcas WHERE idMarca = " + id;
							if (!mysql_query(cn.getConectar(), eliminar.c_str())) {
								cout << "Marca eliminada correctamente.\n";
							}
							else {
								cout << "Error al eliminar la marca.\n";
							}
							cout << "\n¿Desea eliminar otra marca? (s/n): ";
							cin >> confirmar;
						}
						else {
							cout << "Operación cancelada.\n";
							cout << "\n¿Desea buscar otra marca? (s/n): ";
							cin >> confirmar;
						}
					}
					else {
						cout << "No se encontró ninguna marca con ese ID.\n";
						cout << "\n¿Desea intentar con otro ID? (s/n): ";
						cin >> confirmar;
					}
					mysql_free_result(resultado);
				}
				else {
					cout << "Error al consultar la marca.\n";
					confirmar = 'n';
				}
			}
			else {
				cout << "Error en la conexión.\n";
				confirmar = 'n';
			}
			cn.cerrar_conexion();
		} while (confirmar == 's' || confirmar == 'S');
	}

};