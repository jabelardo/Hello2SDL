//
// Created by Jose Gutierrez on 10/18/15.
//

#include "XmlUtils.h"

xmlNode *
getXmlElement(xmlNode *a_node, const xmlChar *name) {
  for (auto cur_node = a_node; cur_node; cur_node = cur_node->next) {
    if (cur_node->type == XML_ELEMENT_NODE && xmlStrcmp(cur_node->name, name) == 0) {
      return cur_node;
    }
  }
  return a_node->children ? getXmlElement(a_node->children, name) : 0;
}