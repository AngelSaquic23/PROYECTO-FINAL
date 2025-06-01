#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <mysql.h>

// Inclusión de clases del sistema
#include "Proveedores.h"
#include "Productos.h"
#include "Clientes.h"
#include "Puestos.h"
#include "Ventas.h"
#include "Compras.h"
#include "Marcas.h"
#include "empleados.h"
#include "gotoxy.h"

using namespace std;

// Instancia global de clases
Cliente* clie = new Cliente();
Puestos* pue = new Puestos();
Ventas* supermerc = new Ventas();
Proveedores* prov = new Proveedores();
empleados* emp = new empleados();
Marcas* marc = new Marcas();
Productos* prod = new Productos();
Compras* com = new Compras();

// Declaraciones
void menu();
void crud(string tipo, int opcion);
void mostrarventa();

int main() {
    menu();
    return 0;
}

// Función principal del menú
void menu() {
    int opcion = 0;
    do {
        system("cls");
        gotoxy(15, 10); cout << "__________MENU__________";
        gotoxy(15, 12); cout << "1. Ingresar";
        gotoxy(15, 13); cout << "2. Mostrar";
        gotoxy(15, 14); cout << "3. Modificar";
        gotoxy(15, 15); cout << "4. Eliminar";
        gotoxy(15, 16); cout << "5. Salir";
        gotoxy(15, 18); cout << "Ingrese opción: ";
        gotoxy(32, 18); cin >> opcion;

        switch (opcion) {
        case 1: crud("INGRESAR", 1); break;
        case 2: crud("MOSTRAR", 2); break;
        case 3: crud("MODIFICAR", 3); break;
        case 4: crud("ELIMINAR", 4); break;
        case 5: exit(0);
        default:
            cout << "Opción inválida..." << endl;
            system("pause");
            break;
        }
    } while (opcion != 5);
}


// Submenú para operaciones CRUD
void crud(string tipo, int opcion) {
    system("cls");
    int opcion_submenu = 0;

    gotoxy(20, 8); cout << tipo << " EN:";
    gotoxy(20, 10); cout << "(1) Clientes";
    gotoxy(20, 11); cout << "(2) Empleados";
    gotoxy(20, 12); cout << "(3) Puestos";
    gotoxy(20, 13); cout << "(4) Productos";
    gotoxy(20, 14); cout << "(5) Marcas";
    gotoxy(20, 15); cout << "(6) Proveedores";
    gotoxy(20, 16); cout << "(7) Ventas";
    gotoxy(20, 17); cout << "(8) Compras";
    gotoxy(20, 18); cout << "(9) Regresar al menu";
    gotoxy(20, 20); cout << "Ingrese opción: ";
    gotoxy(38, 20); cin >> opcion_submenu;

    switch (opcion_submenu) {
    case 1:
        switch (opcion) {
        case 1: system("cls");clie->crear(); break;
        case 2: system("cls");clie->leer(); break;
        case 3: system("cls");clie->actualizar(); break;
        case 4: system("cls");clie->eliminar(); break;
        }
        break;
    case 2:
        switch (opcion) {
        case 1: system("cls");emp->crear(); break;
        case 2: system("cls");emp->leer(); break;
        case 3: system("cls");emp->actualizar(); break;
        case 4: system("cls");emp->eliminar(); break;
        }
        break;
    case 3:
        switch (opcion) {
        case 1: system("cls");pue->crear(); break;
        case 2: system("cls");pue->leer(); break;
        case 3: system("cls");pue->actualizar(); break;
        case 4: system("cls");pue->eliminar(); break;
        }
        break;
    case 4:
        switch (opcion) {
        case 1: system("cls");prod->crear(); break;
        case 2: system("cls");prod->leer(); break;
        case 3: system("cls");prod->actualizar(); break;
        case 4: system("cls");prod->eliminar(); break;
        }
        break;
    case 5:
        switch (opcion) {
        case 1: system("cls");marc->crear(); break;
        case 2: system("cls");marc->leer(); break;
        case 3: system("cls");marc->actualizar(); break;
        case 4: system("cls");marc->eliminar(); break;
        }
        break;
    case 6:
        switch (opcion) {
        case 1: system("cls");prov->crear(); break;
        case 2: system("cls");prov->leer(); break;
        case 3: system("cls");prov->actualizar(); break;
        case 4: system("cls");prov->eliminar(); break;
        }
        break;
    case 7:
        switch (opcion) {
        case 1:
            system("cls");
            supermerc->crear();
            break;
        case 2:
            system("cls");
            mostrarventa();
            break;
        case 3:
            system("cls");
            supermerc->actualizar();
            break;
        case 4:
            system("cls");
            supermerc->eliminar();

            break;
        default:
            cout << "Opción no válida para ventas." << endl;
            system("pause");
            break;
        }
        break;

    case 8:
        switch (opcion) {
        case 1: system("cls");com->crear(); break;
        case 2: system("cls");com->leer(); break;
        case 3: system("cls");com->actualizar(); break;
        case 4: system("cls");com->eliminar(); break;
        }
        break;
    case 9:
        return;
    default:
        cout << "Opción inválida..." << endl;
        break;
    }

    system("pause");
    menu();  // Vuelve al menú principal después de ejecutar
}

// Función para mostrar venta por número de factura
void mostrarventa() {
    supermerc->mostrar();
    cout << "\nPresione una tecla para continuar...";
    system("pause>nul");
}
