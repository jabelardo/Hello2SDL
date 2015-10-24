//
// Created by Jose Gutierrez on 10/18/15.
//

#include "XmlUtils.h"

xmlNode *
getXmlElement(xmlNode *node, const xmlChar *name) {
  for (xmlNode *curNode = node; curNode; curNode = curNode->next) {
    if (curNode->type == XML_ELEMENT_NODE && xmlStrcmp(curNode->name, name) == 0) {
      return curNode;
    }
  }
  return node->children ? getXmlElement(node->children, name) : 0;
}

bool
xmlGetProp(xmlNode *node, const xmlChar *name, int *value) {
  xmlChar *prop = xmlGetProp(node, name);
  if (!prop) {
    return false;
  }
  *value = atoi((const char *) prop);
  xmlFree(prop);
  return true;
}