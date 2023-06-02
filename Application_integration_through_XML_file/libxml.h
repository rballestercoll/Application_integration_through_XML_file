#ifndef LIBXML_H_INCLUDED
#define LIBXML_H_INCLUDED


void pasos();
int menu();
void solicitarAdaptador();
void temporales(char* adaptador);
void obtenerDatos(char* rutaAdaptadores, char* ip, char* mascara, char* pEnlace, char* dns);
void ping(char* dns, char* media);
int saltos(char* dns);
void modificarTracert();
void lineas_vacias();
void archivoXML(char* ip, char* mascara, char* pEnlace, char* dns, char* media, int numSaltos);
void eliminar_temporales();
void limpiar_terminal();

#pragma once

#endif // LIBXML_H_INCLUDED
