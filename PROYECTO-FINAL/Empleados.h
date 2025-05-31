#include "conexionsql.h"
#include <iostream>
#include <cstdlib>
#include <iomanip> // Para setw y left
#include <string>
#include <mysql.h>

using namespace std;

class empleados {
private:
	// idEmpleado no se usa porque es auto_increment
	string idPuesto, nombres, apellidos, direccion,
		fecha_ingreso, dpi, fecha_nacimiento, fecha_inicio, telefono;

	int genero = 0;  // bit: 0 = Mujer, 1 = Hombre

public:
	empleados() {}

	empleados(string _direccion, string nom, string ape, int gen, string tel,
		string fi, string _dpi, string _fecha_nacimiento,
		string _idPuesto, string _fecha_inicio) {
		direccion = _direccion;
		nombres = nom;
		apellidos = ape;
		genero = gen;
		telefono = tel;
		fecha_ingreso = fi;
		dpi = _dpi;
		fecha_nacimiento = _fecha_nacimiento;
		idPuesto = _idPuesto;
		fecha_inicio = _fecha_inicio;
	}

	void crear() {
		int q_estado;
		conexion cn = conexion();
		cn.abrir_conexion();

		if (cn.getConectar()) {
			cout << "Nombres: ";
			cin.ignore();
			getline(cin, nombres);

			cout << "Apellidos: ";
			getline(cin, apellidos);

			cout << "Genero (1 = Hombre, 0 = Mujer): ";
			cin >> genero;
			cin.ignore();

			cout << "Dirección: ";
			getline(cin, direccion);

			cout << "Teléfono: ";
			getline(cin, telefono);

			cout << "DPI: ";
			getline(cin, dpi);

			cout << "Fecha de nacimiento (YYYY-MM-DD): ";
			getline(cin, fecha_nacimiento);

			cout << "ID del puesto: ";
			getline(cin, idPuesto);

			cout << "Fecha de inicio de labores (YYYY-MM-DD): ";
			getline(cin, fecha_inicio);

			cout << "Fecha de ingreso (YYYY-MM-DD): ";
			getline(cin, fecha_ingreso);

			string insert = "INSERT INTO empleados(nombres, apellidos, direccion, telefono, DPI, genero, fecha_nacimiento, idPuesto, fecha_inicio_labores, fechaingreso) VALUES ('" +
				nombres + "', '" + apellidos + "', '" + direccion + "', '" + telefono + "', '" + dpi + "', " + to_string(genero) + ", '" + fecha_nacimiento + "', '" + idPuesto + "', '" + fecha_inicio + "', '" + fecha_ingreso + "')";

			const char* i = insert.c_str();
			q_estado = mysql_query(cn.getConectar(), i);

			if (!q_estado) {
				cout << "Ingreso exitoso.\n";
			}
			else {
				cout << "Error al ingresar los datos.\n";
				cout << "Consulta SQL: " << insert << endl;  // Para depuración
			}
		}
		else {
			cout << "Error en la conexión.\n";
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
			// Consulta con INNER JOIN para mostrar el nombre del puesto y recortar la fecha en fechaingreso
			string consulta = R"(
            SELECT e.idEmpleado, e.nombres, e.apellidos, e.genero, e.direccion,
                   e.telefono, e.DPI, e.fecha_nacimiento, p.puesto,
                   e.fecha_inicio_labores, DATE(e.fechaingreso)
            FROM empleados e
            INNER JOIN puestos p ON e.idPuesto = p.idPuesto
        )";

			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());

				// Encabezado
				cout << "------------------------------- EMPLEADOS -------------------------------" << endl;
				cout << left
					<< setw(5) << "ID"
					<< setw(15) << "Nombres"
					<< setw(15) << "Apellidos"
					<< setw(10) << "Genero"
					<< setw(25) << "Direccion"
					<< setw(12) << "Telefono"
					<< setw(15) << "DPI"
					<< setw(12) << "Nacimiento"
					<< setw(15) << "Puesto"
					<< setw(12) << "Inicio"
					<< setw(12) << "Ingreso" << endl;

				cout << string(145, '-') << endl;

				while ((fila = mysql_fetch_row(resultado))) {
					// Interpretar el bit de género correctamente
					string genero_str = "Mujer";  // default
					if (fila[3] != nullptr && fila[3][0] == 1) {
						genero_str = "Hombre";
					}

					cout << left
						<< setw(5) << (fila[0] ? fila[0] : "")
						<< setw(15) << (fila[1] ? fila[1] : "")
						<< setw(15) << (fila[2] ? fila[2] : "")
						<< setw(10) << genero_str
						<< setw(25) << (fila[4] ? fila[4] : "")
						<< setw(12) << (fila[5] ? fila[5] : "")
						<< setw(15) << (fila[6] ? fila[6] : "")
						<< setw(12) << (fila[7] ? fila[7] : "")
						<< setw(15) << (fila[8] ? fila[8] : "")
						<< setw(12) << (fila[9] ? fila[9] : "")
						<< setw(12) << (fila[10] ? fila[10] : "")
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

	void actualizar() {
		conexion cn = conexion();
		cn.abrir_conexion();

		if (cn.getConectar()) {
			int q_estado;
			string idEmpleado;

			cout << "Ingrese el ID del empleado a consultar: ";
			cin >> idEmpleado;
			cin.ignore();

			string consulta = "SELECT nombres, apellidos, direccion, telefono, DPI, genero, fecha_nacimiento, fecha_inicio_labores, idPuesto, fechaingreso FROM empleados WHERE idEmpleado = " + idEmpleado;
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
				MYSQL_ROW fila = mysql_fetch_row(resultado);

				if (fila) {
					string nom_actual = fila[0] ? fila[0] : "";
					string ape_actual = fila[1] ? fila[1] : "";
					string dir_actual = fila[2] ? fila[2] : "";
					string tel_actual = fila[3] ? fila[3] : "";
					string dpi_actual = fila[4] ? fila[4] : "";
					string genero_actual = (fila[5] != nullptr && fila[5][0] != 0) ? "1" : "0";
					string fnac_actual = fila[6] ? fila[6] : "";
					string finicio_actual = fila[7] ? fila[7] : "";
					string idPuesto_actual = fila[8] ? fila[8] : "";
					string fingreso_actual = fila[9] ? fila[9] : "";

					// Mostrar tabla con datos actuales
					cout << "\n------------------ DATOS ACTUALES DEL EMPLEADO ------------------" << endl;
					cout << left
						<< setw(15) << "Nombres"
						<< setw(15) << "Apellidos"
						<< setw(25) << "Direccion"
						<< setw(12) << "Telefono"
						<< setw(15) << "DPI"
						<< setw(10) << "Genero"
						<< setw(12) << "Nacimiento"
						<< setw(12) << "Inicio"
						<< setw(12) << "Ingreso"
						<< setw(8) << "Puesto" << endl;

					cout << string(140, '-') << endl;
					cout << left
						<< setw(15) << nom_actual
						<< setw(15) << ape_actual
						<< setw(25) << dir_actual
						<< setw(12) << tel_actual
						<< setw(15) << dpi_actual
						<< setw(10) << (genero_actual == "1" ? "Hombre" : "Mujer")
						<< setw(12) << fnac_actual
						<< setw(12) << finicio_actual
						<< setw(12) << fingreso_actual
						<< setw(8) << idPuesto_actual << endl;

					// Confirmar actualizacion
					char opcion_actualizar;
					cout << "\nDesea actualizar estos datos? (s/n): ";
					cin >> opcion_actualizar;
					cin.ignore();

					if (opcion_actualizar == 's' || opcion_actualizar == 'S') {
						// Solicitar nuevos valores
						string nom, ape, dir, tel, dpi, genero, fnac, finicio, fingreso, idPuesto;

						cout << "Nuevo nombre (Enter para conservar): ";
						getline(cin, nom);
						if (nom.empty()) nom = nom_actual;

						cout << "Nuevo apellido (Enter para conservar): ";
						getline(cin, ape);
						if (ape.empty()) ape = ape_actual;

						cout << "Nueva direccion (Enter para conservar): ";
						getline(cin, dir);
						if (dir.empty()) dir = dir_actual;

						cout << "Nuevo telefono (Enter para conservar): ";
						getline(cin, tel);
						if (tel.empty()) tel = tel_actual;

						cout << "Nuevo DPI (Enter para conservar): ";
						getline(cin, dpi);
						if (dpi.empty()) dpi = dpi_actual;

						cout << "Nuevo genero (1=Hombre, 0=Mujer, Enter para conservar): ";
						getline(cin, genero);
						if (genero.empty()) genero = genero_actual;

						cout << "Nueva fecha de nacimiento (YYYY-MM-DD, Enter para conservar): ";
						getline(cin, fnac);
						if (fnac.empty()) fnac = fnac_actual;

						cout << "Nueva fecha inicio labores (YYYY-MM-DD, Enter para conservar): ";
						getline(cin, finicio);
						if (finicio.empty()) finicio = finicio_actual;

						cout << "Nueva fecha de ingreso (YYYY-MM-DD, Enter para conservar): ";
						getline(cin, fingreso);
						if (fingreso.empty()) fingreso = fingreso_actual;

						cout << "Nuevo ID del puesto (Enter para conservar): ";
						getline(cin, idPuesto);
						if (idPuesto.empty()) idPuesto = idPuesto_actual;

						// Armar y ejecutar consulta UPDATE
						string update = "UPDATE empleados SET "
							"nombres = '" + nom +
							"', apellidos = '" + ape +
							"', direccion = '" + dir +
							"', telefono = '" + tel +
							"', DPI = '" + dpi +
							"', genero = " + genero +
							", fecha_nacimiento = '" + fnac +
							"', fecha_inicio_labores = '" + finicio +
							"', fechaingreso = '" + fingreso +
							"', idPuesto = '" + idPuesto +
							"' WHERE idEmpleado = " + idEmpleado;

						const char* i = update.c_str();
						q_estado = mysql_query(cn.getConectar(), i);

						if (!q_estado) {
							cout << "\nRegistro actualizado correctamente.\n";
						}
						else {
							cout << "\nError al actualizar el registro.\n";
							cout << "Consulta SQL: " << update << endl;
						}
					}
					else {
						cout << "\nNo se realizaron cambios.\n";
					}

					// Continuar?
					char opcion;
					cout << "\nDesea consultar otro ID? (s/n): ";
					cin >> opcion;
					cin.ignore();

					if (opcion == 's' || opcion == 'S') {
						actualizar();
					}
					else {
						system("cls");
						system("PROYECTO-FINAL.exe");
					}
				}
				else {
					cout << "No se encontro el empleado con ese ID." << endl;
				}

				mysql_free_result(resultado);
			}
			else {
				cout << "Error al consultar los datos del empleado." << endl;
			}
		}
		else {
			cout << "Error en la conexion." << endl;
		}

		cn.cerrar_conexion();
	}

	void eliminar() {
		conexion cn = conexion();
		cn.abrir_conexion();

		if (cn.getConectar()) {
			int q_estado;
			string idEmpleado;

			cout << "Ingrese el ID del empleado que desea eliminar: ";
			cin >> idEmpleado;
			cin.ignore();

			// Consulta para obtener los datos del empleado con su puesto
			string consulta = R"(
            SELECT e.idEmpleado, e.nombres, e.apellidos, e.genero, e.direccion,
                   e.telefono, e.DPI, e.fecha_nacimiento, p.puesto,
                   e.fecha_inicio_labores, DATE(e.fechaingreso)
            FROM empleados e
            INNER JOIN puestos p ON e.idPuesto = p.idPuesto
            WHERE e.idEmpleado = )" + idEmpleado;

			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
				MYSQL_ROW fila = mysql_fetch_row(resultado);

				if (fila) {
					// Encabezado
					cout << "\n------------------------------ EMPLEADO A ELIMINAR ------------------------------" << endl;
					cout << left
						<< setw(5) << "ID"
						<< setw(15) << "Nombres"
						<< setw(15) << "Apellidos"
						<< setw(10) << "Genero"
						<< setw(25) << "Direccion"
						<< setw(12) << "Telefono"
						<< setw(15) << "DPI"
						<< setw(12) << "Nacimiento"
						<< setw(15) << "Puesto"
						<< setw(12) << "Inicio"
						<< setw(12) << "Ingreso" << endl;

					cout << string(145, '-') << endl;

					string genero_str = "Mujer";
					if (fila[3] != nullptr && fila[3][0] == '1') {
						genero_str = "Hombre";
					}

					cout << left
						<< setw(5) << (fila[0] ? fila[0] : "")
						<< setw(15) << (fila[1] ? fila[1] : "")
						<< setw(15) << (fila[2] ? fila[2] : "")
						<< setw(10) << genero_str
						<< setw(25) << (fila[4] ? fila[4] : "")
						<< setw(12) << (fila[5] ? fila[5] : "")
						<< setw(15) << (fila[6] ? fila[6] : "")
						<< setw(12) << (fila[7] ? fila[7] : "")
						<< setw(15) << (fila[8] ? fila[8] : "")
						<< setw(12) << (fila[9] ? fila[9] : "")
						<< setw(12) << (fila[10] ? fila[10] : "") << endl;

					// Confirmación de eliminación
					char confirmacion;
					cout << "\n¿Está seguro que desea eliminar este empleado? (s/n): ";
					cin >> confirmacion;
					cin.ignore();

					if (confirmacion == 's' || confirmacion == 'S') {
						string eliminar = "DELETE FROM empleados WHERE idEmpleado = " + idEmpleado;
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
						cout << "\n¿Desea hacer otra eliminación? (s = sí / n = menú principal): ";
						cin >> opcion;
						cin.ignore();

						if (opcion == 's' || opcion == 'S') {
							mysql_free_result(resultado);
							cn.cerrar_conexion();
							eliminar(); // llamada recursiva para otro intento
							return;
						}
						else {
							cout << "Regresando al menú principal...\n";
							system("start PROYECTO-FINAL.exe");  // Ejecuta el menú principal
							exit(0);  // Finaliza el proceso actual para evitar duplicidad
						}
					}
				}
				else {
					cout << "No se encontró ningún empleado con ese ID.\n";
				}

				mysql_free_result(resultado);
			}
			else {
				cout << "Error al consultar la base de datos.\n";
			}
		}
		else {
			cout << "Error en la conexión.\n";
		}

		cn.cerrar_conexion();
	}

};