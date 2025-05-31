#include <iomanip>
#include <string>
#include "Conexionsql.h"

using namespace std;
class Puestos {
private:
string puesto;
public:
	Puestos() {
	}
	Puestos(string lpuesto) {
		puesto = lpuesto;
	}
	void crear() {
		int q_estado;
		conexion cn = conexion();
		cn.abrir_conexion();
		if (cn.getConectar()) {
			string puesto;
			char confirmar;

			cout << "\n----- CREAR NUEVO PUESTO -----" << endl;
			cin.ignore();
			cout << "Puesto: ";
			getline(cin, puesto);

			// Mostrar información antes de insertar
			cout << "\n----- VERIFICAR INFORMACIÓN -----" << endl;
			cout << left << setw(20) << "Puesto" << endl;
			cout << left << setw(20) << puesto << endl;

			cout << "\n¿Desea guardar este registro? (S/N): ";
			cin >> confirmar;

			if (toupper(confirmar) == 'S') {
				string insert = "INSERT INTO puestos (puesto) VALUES ('" + puesto + "')";
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
				cout << "Registro cancelado por el usuario." << endl;
			}
		}
		else {
			cout << "Error en la Conexion..." << endl;
		}
		cn.cerrar_conexion();

		// Repetir o salir
		char repetir;
		cout << "\n¿Desea ingresar otro puesto? (S/N): ";
		cin >> repetir;
		if (toupper(repetir) == 'S') {
			cin.ignore(); // Limpiar buffer
			crear();      // Llamada recursiva
		}
	}

	void leer() {
		int q_estado;
		conexion cn = conexion();
		MYSQL_ROW fila;
		MYSQL_RES* resultado;
		cn.abrir_conexion();

		if (cn.getConectar()) {
			string consulta = "SELECT * FROM puestos";
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());

				cout << "\n---------- LISTADO DE PUESTOS ----------\n";
				cout << left << setw(10) << "ID" << setw(30) << "Puesto" << endl;
				cout << "----------------------------------------\n";

				while ((fila = mysql_fetch_row(resultado))) {
					cout << left << setw(10) << fila[0] << setw(30) << fila[1] << endl;
				}

				cout << "----------------------------------------\n";
			}
			else {
				cout << "Error en la consulta." << endl;
			}
		}
		else {
			cout << "Error en la Conexion." << endl;
		}

		cn.cerrar_conexion();
	}

	void actualizar() {
		conexion cn = conexion();
		cn.abrir_conexion();

		if (cn.getConectar()) {
			int q_estado;
			string idpuesto, nuevo_puesto;

			cout << "\n---------- ACTUALIZAR PUESTO ----------" << endl;

			// Mostrar los puestos actuales
			MYSQL_ROW fila;
			MYSQL_RES* resultado;
			string consulta = "SELECT * FROM puestos";
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());
				cout << left << setw(10) << "ID" << setw(30) << "PUESTO" << endl;
				cout << "----------------------------------------\n";
				while ((fila = mysql_fetch_row(resultado))) {
					cout << left << setw(10) << fila[0] << setw(30) << fila[1] << endl;
				}
				cout << "----------------------------------------\n";
			}
			else {
				cout << "Error al obtener los puestos.\n";
				return;
			}

			// Solicitar ID a modificar
			cout << "Ingrese el ID del puesto a modificar: ";
			cin >> idpuesto;

			// Solicitar nuevo nombre
			cin.ignore();
			cout << "Ingrese el nuevo nombre del puesto: ";
			getline(cin, nuevo_puesto);

			// Confirmar actualizacion
			char confirm;
			cout << "\nDesea actualizar el puesto con ID " << idpuesto << " a '" << nuevo_puesto << "' ? (s/n): ";
			cin >> confirm;

			if (confirm == 's' || confirm == 'S') {
				string update = "UPDATE puestos SET puesto = '" + nuevo_puesto + "' WHERE idpuesto = " + idpuesto;
				const char* i = update.c_str();
				q_estado = mysql_query(cn.getConectar(), i);
				if (!q_estado) {
					cout << "Puesto actualizado correctamente.\n";
				}
				else {
					cout << "Error al actualizar el puesto.\n";
				}
			}
			else {
				cout << "Operacion cancelada por el usuario.\n";
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
			string idpuesto;

			cout << "\n---------- ELIMINAR PUESTO ----------" << endl;

			// Mostrar los puestos actuales
			MYSQL_ROW fila;
			MYSQL_RES* resultado;
			string consulta = "SELECT * FROM puestos";
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());
				cout << left << setw(10) << "ID" << setw(30) << "PUESTO" << endl;
				cout << "----------------------------------------\n";
				while ((fila = mysql_fetch_row(resultado))) {
					cout << left << setw(10) << fila[0] << setw(30) << fila[1] << endl;
				}
				cout << "----------------------------------------\n";
			}
			else {
				cout << "Error al obtener los puestos.\n";
				return;
			}

			// Solicitar ID a eliminar
			cout << "Ingrese el ID del puesto a eliminar: ";
			cin >> idpuesto;

			// Confirmar eliminacion
			char confirm;
			cout << "\nEsta seguro que desea eliminar el puesto con ID " << idpuesto << " ? (s/n): ";
			cin >> confirm;

			if (confirm == 's' || confirm == 'S') {
				string eliminar = "DELETE FROM puestos WHERE idpuesto = " + idpuesto;
				const char* i = eliminar.c_str();
				q_estado = mysql_query(cn.getConectar(), i);
				if (!q_estado) {
					cout << "Puesto eliminado correctamente.\n";
				}
				else {
					cout << "Error al eliminar el puesto.\n";
				}
			}
			else {
				cout << "Operacion cancelada por el usuario.\n";
			}

		}
		else {
			cout << "Error en la conexion.\n";
		}

		cn.cerrar_conexion();
	}

};