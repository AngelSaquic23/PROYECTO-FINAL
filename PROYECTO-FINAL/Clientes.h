#pragma once
#include <cstring>
#include <string>
#include <iomanip>
#include "Conexionsql.h"
#include "Ventas.h"
using namespace std;
class Cliente {

private:
	string nit, nombres, apellidos, genero, correo_electronico, fecha_ingreso;
	int telefono = 0;

public:
	Cliente() {
	}
	Cliente(string nomb, string apell, string sex, int tele, string correo, string fi, string n) {
		nit = n;
		nombres = nomb;
		apellidos = apell;
		genero = sex;
		correo_electronico = correo;
		telefono = tele;
		fecha_ingreso = fi;
	}

	void crear() const {
		int q_estado;
		conexion cn = conexion();
		cn.abrir_conexion();

		if (cn.getConectar()) {
			string t = to_string(telefono);
			string nit, nombres, apellidos, correo_electronico, fecha_ingreso;
			int genero;  // tipo entero para bit(1)

			cin.ignore();
			cout << "Ingrese NIT: ";
			getline(cin, nit);
			cout << "Ingrese Nombres: ";
			getline(cin, nombres);
			cout << "Ingrese Apellidos: ";
			getline(cin, apellidos);
			cout << "Ingrese genero (1 = Hombre, 0 = Mujer): ";
			cin >> genero;
			cout << "Ingrese Telefono: ";
			cin >> t;
			cin.ignore(); // Limpiar buffer
			cout << "Ingrese Correo Electronico: ";
			getline(cin, correo_electronico);
			cout << "Ingrese Fecha de Ingreso (YYYY-MM-DD), tambien puede poner la hora HH:MM:SS (Opcional): ";
			getline(cin, fecha_ingreso);  // Solo fecha

			// Construcción de la consulta SQL
			string insert = "INSERT INTO Clientes(nit,nombres,apellidos,genero,telefono,correo_electronico,fechaingreso) VALUES ('" +
				nit + "','" +
				nombres + "','" +
				apellidos + "'," +
				to_string(genero) + ",'" +
				t + "','" +
				correo_electronico + "','" +
				fecha_ingreso + "');";

			const char* i = insert.c_str();
			q_estado = mysql_query(cn.getConectar(), i);

			if (!q_estado) {
				cout << "Ingreso Exitoso...." << endl;
			}
			else {
				cout << "---- Error al Ingresar ----" << endl;
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
			string consulta = "SELECT * FROM clientes";
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());

				// Encabezado de columna
				cout << "----------------------------- CLIENTES -----------------------------" << endl;
				cout << left
					<< setw(5) << "ID"
					<< setw(15) << "NIT"
					<< setw(20) << "Nombres"
					<< setw(20) << "Apellidos"
					<< setw(8) << "Genero"
					<< setw(15) << "Telefono"
					<< setw(30) << "Correo"
					<< setw(20) << "Fecha Ingreso" << endl;

				cout << string(133, '-') << endl;

				while ((fila = mysql_fetch_row(resultado))) {
					// Aquí está la corrección clave:
					string genero_str = (fila[4] != nullptr && fila[4][0] != 0) ? "Hombre" : "Mujer";

					cout << left
						<< setw(5) << fila[0]
						<< setw(15) << fila[3]
						<< setw(20) << fila[1]
						<< setw(20) << fila[2]
						<< setw(8) << genero_str
						<< setw(15) << fila[5]
						<< setw(30) << fila[6]
						<< setw(20) << fila[7]
						<< endl;
				}
			}
			else {
				cout << "---- Error al Consultar ----" << endl;
			}
		}
		else {
			cout << "Error en la Conexion..." << endl;
		}
		cn.cerrar_conexion();
	}

	void actualizar() const {
		conexion cn = conexion();
		cn.abrir_conexion();
		if (cn.getConectar()) {
			int q_estado;
			string id;
			cout << "Ingrese el ID del Cliente que desea actualizar: ";
			cin >> id;
			cin.ignore();

			// Obtener los datos actuales
			string consulta = "SELECT nombres, apellidos, nit, genero, telefono, correo_electronico, fechaingreso FROM clientes WHERE idCliente = " + id;
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
				MYSQL_ROW fila = mysql_fetch_row(resultado);

				if (fila) {
					string nom_actual = fila[0] ? fila[0] : "";
					string ape_actual = fila[1] ? fila[1] : "";
					string nit_actual = fila[2] ? fila[2] : "";
					string genero_actual = (fila[3] != nullptr && fila[3][0] != 0) ? "1" : "0";
					string tel_actual = fila[4] ? fila[4] : "";
					string correo_actual = fila[5] ? fila[5] : "";
					string fi_actual = fila[6] ? fila[6] : "";

					// Mostrar en columnas
					cout << "\n-------------- DATOS ACTUALES DEL CLIENTE --------------\n";
					cout << left << setw(15) << "Nombre"
						<< setw(15) << "Apellido"
						<< setw(12) << "NIT"
						<< setw(10) << "Genero"
						<< setw(15) << "Telefono"
						<< setw(25) << "Correo"
						<< setw(20) << "Fecha Ingreso" << endl;

					cout << left << setw(15) << nom_actual
						<< setw(15) << ape_actual
						<< setw(12) << nit_actual
						<< setw(10) << (genero_actual == "1" ? "Hombre" : "Mujer")
						<< setw(15) << tel_actual
						<< setw(25) << correo_actual
						<< setw(20) << fi_actual << endl;

					string confirm;
					cout << "\n¿Deseas actualizar estos datos? (s/n): ";
					getline(cin, confirm);

					if (confirm == "s" || confirm == "S") {
						string nom, ape, nit, genero, tel, correo, fi;

						cout << "Nuevo nombre (Enter para conservar): ";
						getline(cin, nom);
						if (nom.empty()) nom = nom_actual;

						cout << "Nuevo apellido (Enter para conservar): ";
						getline(cin, ape);
						if (ape.empty()) ape = ape_actual;

						cout << "Nuevo NIT (Enter para conservar): ";
						getline(cin, nit);
						if (nit.empty()) nit = nit_actual;

						cout << "Nuevo genero (1=Hombre, 0=Mujer, Enter para conservar): ";
						getline(cin, genero);
						if (genero.empty()) genero = genero_actual;

						cout << "Nuevo telefono (Enter para conservar): ";
						getline(cin, tel);
						if (tel.empty()) tel = tel_actual;

						cout << "Nuevo correo (Enter para conservar): ";
						getline(cin, correo);
						if (correo.empty()) correo = correo_actual;

						cout << "Nueva fecha de ingreso (YYYY-MM-DD [HH:MM:SS], Enter para conservar): ";
						getline(cin, fi);
						if (fi.empty()) fi = fi_actual;

						string update = "UPDATE clientes SET nombres = '" + nom +
							"', apellidos = '" + ape +
							"', nit = '" + nit +
							"', genero = " + genero +
							", telefono = '" + tel +
							"', correo_electronico = '" + correo +
							"', fechaingreso = '" + fi +
							"' WHERE idCliente = " + id;

						const char* i = update.c_str();
						q_estado = mysql_query(cn.getConectar(), i);

						if (!q_estado) {
							cout << "\nRegistro actualizado exitosamente.\n";
						}
						else {
							cout << "\nError al actualizar el registro.\n";
						}

					}
					else {
						cout << "\nActualizacion cancelada.\n";
					}

					// Consulta repetida o regresar al menú
					string opcion;
					cout << "\n¿Deseas consultar otro ID? (s) o ¿Ir al menu principal? (m): ";
					getline(cin, opcion);
					if (opcion == "s" || opcion == "S") {
						actualizar(); // Llamada recursiva
					}

				}
				else {
					cout << "\nNo se encontro el cliente con el ID ingresado.\n";
				}
			}
			else {
				cout << "\nError al consultar los datos del cliente.\n";
			}
		}
		else {
			cout << "Error en la conexion.\n";
		}
		cn.cerrar_conexion();
	}

	void eliminar() {
		conexion cn = conexion();
		cn.abrir_conexion();

		if (cn.getConectar()) {
			int q_estado;
			string idCliente;

			cout << "Ingrese el ID del cliente que desea eliminar: ";
			cin >> idCliente;
			cin.ignore();

			string consulta = "SELECT * FROM clientes WHERE idCliente = " + idCliente;
			const char* c = consulta.c_str();

			q_estado = mysql_query(cn.getConectar(), c);
			if (!q_estado) {
				MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
				MYSQL_ROW fila = mysql_fetch_row(resultado);

				if (fila) {
					cout << "----------------------------- CLIENTE -----------------------------" << endl;
					cout << left
						<< setw(5) << "ID"
						<< setw(15) << "NIT"
						<< setw(20) << "Nombres"
						<< setw(20) << "Apellidos"
						<< setw(8) << "Genero"
						<< setw(15) << "Telefono"
						<< setw(30) << "Correo"
						<< setw(20) << "Fecha Ingreso" << endl;

					cout << string(133, '-') << endl;

					// Igual que en leer(), género es "Hombre" si fila[4][0] != 0, sino "Mujer"
					string genero_str = (fila[4] != nullptr && fila[4][0] != 0) ? "Hombre" : "Mujer";

					cout << left
						<< setw(5) << fila[0]
						<< setw(15) << fila[3]
						<< setw(20) << fila[1]
						<< setw(20) << fila[2]
						<< setw(8) << genero_str
						<< setw(15) << fila[5]
						<< setw(30) << fila[6]
						<< setw(20) << fila[7]
						<< endl;

					char confirmacion;
					cout << "\nEsta seguro que desea eliminar este cliente? (s/n): ";
					cin >> confirmacion;
					cin.ignore();

					if (confirmacion == 's' || confirmacion == 'S') {
						string eliminar = "DELETE FROM clientes WHERE idCliente = " + idCliente;
						const char* del = eliminar.c_str();
						q_estado = mysql_query(cn.getConectar(), del);
						if (!q_estado) {
							cout << "Registro eliminado correctamente.\n";
						}
						else {
							cout << "Error al eliminar el registro.\n";
						}
					}
					else {
						char opcion;
						cout << "\n¿Desea hacer otra consulta? (s = si / n = menu principal): ";
						cin >> opcion;
						cin.ignore();

						if (opcion == 's' || opcion == 'S') {
							eliminar(); // llamada recursiva para eliminar otro
						}
						else {
							cout << "Regresando al menu principal...\n";
							return;
						}
					}
				}
				else {
					cout << "No se encontró ningún cliente con ese ID.\n";
				}

				mysql_free_result(resultado);
			}
			else {
				cout << "Error al consultar la base de datos.\n";
			}
		}
		else {
			cout << "Error en la conexion.\n";
		}

		cn.cerrar_conexion();
	}

};