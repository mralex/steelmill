/* psmetalobj.h - Power Station Glib PhyMod Library
 * Copyright (c) 2000 David A. Bartold
 * Copyright (c) 2021 Alex Roberts
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __PS_METAL_OBJ_H_
#define __PS_METAL_OBJ_H_

#include "rack.hpp"
#include "vector3.hpp"
#include <cmath>

using namespace rack;

#ifdef __cplusplus
extern "C" {
#endif

typedef
struct _PSMetalObjNode
{
  int anchor;
  Vector3  pos;
  Vector3  vel;

  int                    num_neighbors;
  struct _PSMetalObjNode *neighbors[1];
} PSMetalObjNode;

typedef
struct _PSMetalObj
{
  int            num_nodes;
  PSMetalObjNode *nodes[1];
} PSMetalObj;

void ps_metal_obj_node_free (PSMetalObjNode *n);
PSMetalObj *ps_metal_obj_new (int size);
void ps_metal_obj_free (PSMetalObj *obj);
PSMetalObj *ps_metal_obj_new_tube (int height, int circum, float tension);
void ps_metal_obj_tube_initialize (PSMetalObj *obj, int height, int circum, float tension);
PSMetalObj *ps_metal_obj_new_rod (int height, float tension);
void ps_metal_obj_rod_initialize(PSMetalObj *obj, int height, float tension);
PSMetalObj *ps_metal_obj_new_plane(int length, int width, float tension);
void ps_metal_obj_plane_initialize(PSMetalObj *obj, int length, int width, float tension);
// PSMetalObj *ps_metal_obj_new_hypercube (int dimensions, int size, float tension);

static inline void ps_metal_obj_perturb(PSMetalObj *obj, float speed, float damp)
{
  int i, j;
  Vector3 sum;
  Vector3 dif;
  PSMetalObjNode *inode;
  float temp;
  float sprinps_k;

  for (i = 0; i < obj->num_nodes; i++)
  {
    inode = obj->nodes[i];
    if (!inode->anchor)
    {
      sum = Vector3(0.f);

      for (j = 0; j < inode->num_neighbors; j++)
      {
        dif = inode->pos - inode->neighbors[j]->pos;
        temp = 1.0f - std::sqrt(dif.square());
        sum = sum + dif * temp;
      }

      sprinps_k = 1.0f;
      inode->vel = (inode->vel + sum * speed * sprinps_k) * damp;
    }
  }

  for (i = 0; i < obj->num_nodes; i++)
  {
    inode = obj->nodes[i];

    if (!inode->anchor)
    {
      inode->pos = inode->pos + inode->vel * speed;
    }
  }
}

#ifdef __cplusplus
}
#endif

#endif
