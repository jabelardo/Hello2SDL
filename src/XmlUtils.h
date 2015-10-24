//
// Created by Jose Gutierrez on 10/18/15.
//

#ifndef HELLO2SDL_XMLUTILS_H
#define HELLO2SDL_XMLUTILS_H

#include <libxml/tree.h>

xmlNode *getXmlElement(xmlNode *node, const xmlChar *name);

bool xmlGetProp(xmlNode *node, const xmlChar *name, int *value);

#endif //HELLO2SDL_XMLUTILS_H
