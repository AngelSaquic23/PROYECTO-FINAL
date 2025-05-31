
#pragma once

#include <string>
#include <time.h>
#include "gotoxy.h"
#include "Conexionsql.h"
using namespace std;

class Ventas
{
protected: int idcl, idemp, idpr;
		 string Nodefact;
		string fechafac;
public:

	struct registroVentas {
		float cantidad = 0;
		float precio = 0;
		float total1 = 0;
		float totalaPagar2 = 0;
		int nofactura = 1;
	}factura[100];

	string fecha() {
		time_t now = time(0);
		tm time;
		char buf[80];
		time = *localtime(&now);

		strftime(buf, sizeof(buf), "%Y-%m-%d", &time);
		return buf;
	}

	conexion cn = conexion();
	int q_estado;
	MYSQL_ROW fila;
	MYSQL_RES* resultado;
	registroVentas registro;

	short z, x = 14, y = 10, f, cantcompra;
	
	void buscarcl(string nit) {
		
		char addnit;

		cn.abrir_conexion();

		if (cn.getConectar()) {
			string consulta = "SELECT * FROM clientes WHERE nit = '" + nit + "'";
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());
				
				uint64_t rows = mysql_num_rows(resultado);

				if (mysql_num_rows(resultado) > 0) {
					while (fila = mysql_fetch_row(resultado)) {
						gotoxy(25, 4);  cout << "Nombres: " << fila[1] << " " << fila[2] << endl;
						int id = atoi(fila[0]);
						int id_empl;

						gotoxy(65, 4); cout << "Ingrese su Id: ";
						cin >> id_empl;

						ingresarventa(id, id_empl);
					}
				}
				else {
					cout << "NIT no encotrado, agreagar? S/N: ";
					cin >> addnit;
					if (addnit == 'S') {
						cout << "Agregar ";
					}
					else if(addnit == 'N')
					{
						string nit;
						cout << "ingresar el NIT: ";
						
						cin >> nit;
						buscarcl(nit);
					}
				}
			}
			else {
				cout << "Error al consulta" << endl;
				cout << consulta << endl << mysql_error(cn.getConectar()) << endl;
			}
		}
		else {
			cout << "Conexion error" << endl;
		}
	}

	void ingresarventa(int id_c, int id_e) {
		cn.abrir_conexion();
		if (cn.getConectar()) {
			int nf1 = numerofact();
			string nf = to_string(nf1 + 1);
			string idcl1 = to_string(id_c);
			string idem = to_string(id_e);

			string insertv = "INSERT INTO ventas(nofactura, serie, fechafactura, idcliente, idempleado, fechaingreso) VALUES (" + nf + ", 'A', '" + fecha() + "', " + idcl1 + ", " + idem + ", now()); ";
			const char* i = insertv.c_str();
			q_estado = mysql_query(cn.getConectar(), i);

			if (!q_estado) {
				system("cls");
				cout << "Fecha: " << fecha() << "   No.: " << nf << "   No. fac:\n";
				cout << "NIT: " << id_c << "   Nombres: "; mostrarNombreCliente(id_c); // puedes implementar mostrarNombreCliente()
				cout << "\nIngrese su Id: " << id_e << endl;

				// Encabezado de tabla
				gotoxy(5, 6); cout << "ID Producto";
				gotoxy(20, 6); cout << "Producto";
				gotoxy(45, 6); cout << "Cantidad";
				gotoxy(60, 6); cout << "Precio";

				int y = 7;
				char continuar;
				do {
					int id_producto, cantidad;
					float precio_unitario;
					string producto;

					gotoxy(5, y);  cin >> id_producto;
					gotoxy(20, y); cin >> producto;
					gotoxy(45, y); cin >> cantidad;
					gotoxy(60, y); cin >> precio_unitario;

					// Insertar en detalle_ventas
					string insert_det = "INSERT INTO detalle_ventas(idventa, idproducto, cantidad, precio_unitario) VALUES (" + nf + ", " + to_string(id_producto) + ", " + to_string(cantidad) + ", " + to_string(precio_unitario) + ");";
					q_estado = mysql_query(cn.getConectar(), insert_det.c_str());
					if (q_estado) {
						cout << "\nError al insertar detalle: " << mysql_error(cn.getConectar()) << endl;
					}

					y++;
					cout << "\n¿Desea ingresar otro producto? (s/n): ";
					cin >> continuar;
				} while (continuar == 's' || continuar == 'S');

				cout << "\nIngreso exitoso..." << endl;
			}
			else {
				cout << "Error al ingresar venta" << endl;
				cout << insertv << endl << mysql_error(cn.getConectar()) << endl;
			}
		}
		else {
			cout << "Conexion error" << endl;
		}
		cn.cerrar_conexion();
	}

	void mostrarNombreCliente(int id_cliente) {
		cn.abrir_conexion();
		if (cn.getConectar()) {
			string consulta = "SELECT nombres, apellidos FROM clientes WHERE idCliente = " + to_string(id_cliente) + ";";
			const char* q = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), q);
			if (!q_estado) {
				MYSQL_RES* resultado = mysql_store_result(cn.getConectar());
				MYSQL_ROW fila;
				if ((fila = mysql_fetch_row(resultado))) {
					cout << fila[0] << " " << fila[1];  // nombres + apellidos
				}
				else {
					cout << "No encontrado";
				}
			}
			else {
				cout << "Error al consultar cliente: " << mysql_error(cn.getConectar());
			}
		}
		else {
			cout << "Error de conexión.";
		}
		cn.cerrar_conexion();
	}

	void facturar() {
		x = 14;
		y = 10;

		float acumulador = 0;
		int prod, cant = 0, nofactura = 1;
		char SN;
		string str, prodto, canti;

		gotoxy(10, 8); cout << "ID Producto ";
		gotoxy(27, 8); cout << "Producto ";
		gotoxy(57, 8); cout << "Cantidad ";
		gotoxy(72, 8); cout << "Precio ";

		cn.abrir_conexion();

		do {
			gotoxy(x+1, y-1);
			cin >> prod;
			prodto = to_string(prod);
			cin.ignore();
			getline(cin, prodto);

			str = "SELECT * FROM productos WHERE idProducto = " + prodto; cout << endl;
			
		if (cn.getConectar()) {
		const char* c = str.c_str();
		q_estado = mysql_query(cn.getConectar(), c);
		if (!q_estado) {
		resultado = mysql_store_result(cn.getConectar());
		uint64_t rows = mysql_num_rows(resultado);
		if (mysql_num_rows(resultado) > 0) {
		while (fila = mysql_fetch_row(resultado)) {registro.precio = stof(fila[6]);string price = to_string(registro.precio);
		gotoxy(x + 15, y);
		cout << fila[3] << endl;
		gotoxy(x + 47, y);
		cin >> registro.cantidad;
		canti = to_string(registro.cantidad);
		cin.ignore();
		gotoxy(x + 60, y);
		factura[f].total1 = registro.cantidad * registro.precio;
	printf("%.2f", factura[f].total1);
		acumulador += factura[f].total1;
		fflush(stdin);
		gotoxy(x, y+10);
		string str = "INSERT INTO ventas_detalle(idventa, idProducto, cantidad, precio_unitario) VALUES((SELECT MAX(idVenta) FROM ventas), " + prodto + ", " + canti + ", " + price +");";
		const char* i = str.c_str();
		q_estado = mysql_query(cn.getConectar(), i);
		if (!q_estado) {					}
		else {
		cout << "Error al ingresar venta Mdetalle, verificar datos" << endl;
	cout << str << endl << mysql_error(cn.getConectar()) << endl;
	cn.cerrar_conexion();
			}
	}
	}
	}
	}
	else {
	cout << "Conexion error" << endl;
	cn.cerrar_conexion();
			}
			gotoxy(50, 6);
			fflush(stdin);
			factura[f].totalaPagar2 = acumulador;
			gotoxy(55, 8);
			printf("Total: %.2f", acumulador);
			gotoxy(55, 9);
			cout << "desea agregar? (S/N): ";
			gotoxy(60, 10);
			cin >> SN;
			f++;
			y += 2;
		} while (SN == 'S' || SN == 's');
		cn.cerrar_conexion();
	}

	int numerofact() {
		int fact = 0;
		cn.abrir_conexion();
		if (cn.getConectar()) {
			string consulta = "SELECT MAX(nofactura) FROM ventas;";
			const char* c = consulta.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());

				uint64_t rows = mysql_num_rows(resultado);

				if (mysql_num_rows(resultado) > 0) {
					while (fila = mysql_fetch_row(resultado)) {
						fact = atoi(fila[0]);
						gotoxy(25, 3); cout << "No.: " << fact + 1;
					}
				}
				else {
					cout << "NIT no se encuentra , desea agregar S/N: ";
				}
			}
			else {
				cout << "Error" << endl;
				cout << consulta << endl << mysql_error(cn.getConectar()) << endl;
			}
		}
		else {
			cout << "Conexion error" << endl;
		}
		return fact;
	}

	void mostrarventa(int nofact) {

		int prod;
		string prodto, canti;

		cn.abrir_conexion();

		if (cn.getConectar()) {
			string f = to_string(nofact);
			string strv = "SELECT idventa, nofactura, serie, fechafactura, c.nit, c.nombres, c.apellidos FROM ventas v INNER JOIN clientes c ON v.idcliente = c.idCliente WHERE nofactura = " + f;

			const char* c = strv.c_str();
			q_estado = mysql_query(cn.getConectar(), c);

			if (!q_estado) {
				resultado = mysql_store_result(cn.getConectar());
				while (fila = mysql_fetch_row(resultado)) {

					system("cls");
					gotoxy(5, 3);
					cout << "Fecha: "<< fila[3];
					gotoxy(25, 3);
					cout << "Factura No: " << fila[2];
					gotoxy(5, 4);
					cout << "NIT: " << fila[4];
					gotoxy(25, 7);  cout << "Nombre: " << fila[5] << " " << fila[6] << endl;

					gotoxy(10, 8); cout << "Id Producto ";
					gotoxy(27, 8); cout << "Producto ";
					gotoxy(57, 8); cout << "Cantidad ";
					gotoxy(72, 8); cout << "Precio ";

					string idventa = to_string(atoi(fila[0]));

					string strvd = "SELECT vd.idProducto, p.descripcion, vd.cantidad, (vd.cantidad * vd.precio_unitario) AS Precio FROM ventas_detalle vd INNER JOIN productos p ON vd.idProducto = p.idProducto WHERE idventa = " + idventa;

					const char* cc = strvd.c_str();
					q_estado = mysql_query(cn.getConectar(), cc);

					if (!q_estado) {
						resultado = mysql_store_result(cn.getConectar());

						if (mysql_num_rows(resultado) > 0) {
							y = 10;
							x = 15;
							while (fila = mysql_fetch_row(resultado)) {
								
								gotoxy(x, y++); cout << fila[0];
								gotoxy(x + 15, y-1); cout << fila[1];
								gotoxy(x + 47, y - 1); cout << atoi(fila[2]);
								gotoxy(x + 60, y - 1); cout << stof(fila[3]);
							}

							char SN;
							gotoxy(38, 20);
							cout << "Desea modificar productos? (s/n): ";
							cin >> SN;

							if (SN == 'S' || SN == 's') {
								facturar();
								gotoxy(x, 9);
								cin >> prod;
								prodto = to_string(prod);
								cin.ignore();
								gotoxy(x + 15, 9);
								cout << fila[3] << endl;
								gotoxy(x + 47, 9);
								cin >> registro.cantidad;
								canti = to_string(registro.cantidad);
								cin.ignore();
								
							}
						}
					}
					else {
						cout << "Error" << endl;
						cout << strvd << endl << mysql_error(cn.getConectar()) << endl;
					}
				}
			}
			else {
				cout << "Error al consultar" << endl;
			}
		}
		else {
			cout << "Conexion error" << endl;
		}
		cn.cerrar_conexion();

	}

	void eliminarventa() {
		int fact;
		cout << "Ingrese la factura a eliminar: ";
		cin >> fact;
		cn.abrir_conexion();
		if (cn.getConectar()) {

			string f = to_string(fact);
			string eliminard = "DELETE FROM ventas_detalle WHERE idventa = (SELECT idventa FROM ventas WHERE nofactura = '" + f + "');";
			string eliminarv = "DELETE FROM ventas WHERE nofactura = '" + f + "';";

			const char* i = eliminard.c_str();
			q_estado = mysql_query(cn.getConectar(), i);

			if (!q_estado) {
				cout << "Factura eliminada" << endl;
			}
			else {
				cout << "No se puede eliminar" << endl;
				cout << eliminard << endl << mysql_error(cn.getConectar()) << endl;
			}

			const char* ii = eliminarv.c_str();
			q_estado = mysql_query(cn.getConectar(), ii);

			if (!q_estado) {
				cout << "Factura eliminada" << endl;
				return;
			}
			else {
				cout << "No se puede eliminar" << endl;
				cout << eliminarv << endl << mysql_error(cn.getConectar()) << endl;
			}
		}
		

	}
};
