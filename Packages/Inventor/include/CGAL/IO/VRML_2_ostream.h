// Copyright (c) 1997  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Andreas Fabri
//                 Lutz Kettner <kettner@inf.ethz.ch>
//                 Herve Bronnimann <Herve.Bronnimann@sophia.inria.fr>
//                 Mariette Yvinec <Mariette.Yvinec@sophia.inria.fr>



#ifndef CGAL_IO_VRML_2_OSTREAM_H
#define CGAL_IO_VRML_2_OSTREAM_H

#ifndef CGAL_BASIC_H
#include <CGAL/basic.h>
#endif // CGAL_BASIC_H
#ifndef CGAL_PROTECT_IOSTREAM_H
#include <iostream>
#define CGAL_PROTECT_IOSTREAM_H
#endif // CGAL_PROTECT_IOSTREAM_H

CGAL_BEGIN_NAMESPACE

class VRML_2_ostream {
public:
    VRML_2_ostream()           : m_os(0)  {}
    VRML_2_ostream(std::ostream& o) : m_os(&o) { header();}
    ~VRML_2_ostream()  { close(); }
    void open(std::ostream& o)   { m_os = &o; header(); }
    void close() {
        if ( m_os)
            footer();
        m_os = 0;
    }
    typedef const void* Const_void_ptr;
    operator Const_void_ptr () const {
        if ( m_os)
            return *m_os;
        return 0;
    }
    std::ostream& os() {
        // The behaviour if m_os == 0 could be changed to return
        // cerr or a file handle to /dev/null. The latter one would
        // mimick the behaviour that one can still use a stream with
        // an invalid stream, but without producing any output.
        CGAL_assertion( m_os);
        return *m_os;
    }
private:
    void header() {
        os() << "#VRML V2.0 utf8\n"
                "# File written with the help of the CGAL Library\n"
                "#-- Begin of file header\n"
                "Group {\n"
                "    children [\n"
                "        Shape {\n"
                "          appearance DEF A1 Appearance {\n"
                "            material Material {\n"
                "              diffuseColor .6 .5 .9\n"
                "            }\n         }\n"
                "            appearance\n"
                "                Appearance {\n"
                "                    material DEF Material Material {}\n"
                "                }\n"
                "            geometry NULL\n"
                "        }\n"
                "        #-- End of file header" << std::endl;
    }
    void footer() {
        os() << "        #-- Begin of file footer\n"
                "    ]\n"
                "}\n"
                "#-- End of file footer" << std::endl;
    }
    std::ostream*  m_os;
};

VRML_2_ostream&
operator<<(VRML_2_ostream& os,
           const char* s)
{
  os.os() << s;
  return os;
}

VRML_2_ostream&
operator<<(VRML_2_ostream& os,
           const double& d)
{
  os.os() << d;
  return os;
}

CGAL_END_NAMESPACE

#endif // CGAL_IO_VRML_2_OSTREAM_H

#ifdef CGAL_TETRAHEDRON_3_H
#ifndef CGAL_IO_VRML_2_TETRAHEDRON_3
#define CGAL_IO_VRML_2_TETRAHEDRON_3

CGAL_BEGIN_NAMESPACE

template <class R >
VRML_2_ostream&
operator<<(VRML_2_ostream& os,
           const Tetrahedron_3<R > &t)
{
  const char *Indent = "                                    ";
  os <<      "        Group {\n"
             "            children [\n"
             "                Shape {\n"
             "                    appearance\n"
             "                        Appearance {\n"
             "                            material USE Material\n"
             "                        } #Appearance\n"
             "                    geometry\n"
             "                        IndexedFaceSet {\n"
             "                            coord Coordinate {\n"
             "                                point [ \n"
          << Indent << "point [\n"
          << Indent << "  "
          << CGAL::to_double(t[0].x()) << " "
          << CGAL::to_double(t[0].y()) << " "
          << CGAL::to_double(t[0].z()) << " ,\n"
          << Indent << "  "
          << CGAL::to_double(t[1].x()) << " "
          << CGAL::to_double(t[1].y()) << " "
          << CGAL::to_double(t[1].z()) << " ,\n"
          << Indent << "  "
          << CGAL::to_double(t[2].x()) << " "
          << CGAL::to_double(t[2].y()) << " "
          << CGAL::to_double(t[2].z()) << " ,\n"
          << Indent << "  "
          << CGAL::to_double(t[3].x()) << " "
          << CGAL::to_double(t[3].y()) << " "
          << CGAL::to_double(t[3].z()) << " ]"
             "\n                                ]\n"
             "                            }\n"
             "                            solid   FALSE\n"
          << Indent << "coordIndex  [ 0,1,2,-1, 1,3,2,-1,\n"
          << Indent << "              0,2,3,-1, 0,3,1,-1 ]\n"
             "                        } #IndexedFaceSet\n"
             "                } #Shape\n"
             "            ] #children\n"
             "        } #Group\n";
  return os;
}

CGAL_END_NAMESPACE

#endif // CGAL_IO_VRML_2_TETRAHEDRON_3
#endif // CGAL_TETRAHEDRON_3_H

#ifdef CGAL_POINT_3_H
#ifndef CGAL_IO_VRML_2_POINT_3
#define CGAL_IO_VRML_2_POINT_3

CGAL_BEGIN_NAMESPACE

template <class R >
VRML_2_ostream&
operator<<(VRML_2_ostream& os,
           const Point_3<R > &p)
{
  const char *Indent = "                                    ";
  os <<      "        Group {\n"
             "            children [\n"
             "                Shape {\n"
             "                    appearance USE A1\n"
             "                    geometry\n"
             "                        PointSet {\n"
             "                            coord Coordinate {\n"
             "                                point [ ";
  os << CGAL::to_double(p.x()) << " " << CGAL::to_double(p.y()) 
     << " " << CGAL::to_double(p.z()) << " ]\n";
  os << Indent << "}\n";
  os << Indent << "} # PointSet\n";
  os << "                } #Shape\n"
        "            ] #children\n"
        "        } #Group\n";
  return os;
}

CGAL_END_NAMESPACE

#endif // CGAL_IO_VRML_2_POINT_3
#endif // CGAL_POINT_3_H



#ifdef CGAL_TRIANGLE_3_H
#ifndef CGAL_IO_VRML_2_TRIANGLE_3
#define CGAL_IO_VRML_2_TRIANGLE_3

CGAL_BEGIN_NAMESPACE

template <class R >
VRML_2_ostream&
operator<<(VRML_2_ostream& os,
           const Triangle_3<R > &t)
{
  const char *Indent = "                                    ";
  os <<      "        Group {\n"
             "            children [\n"
             "                Shape {\n"
             "                    appearance USE A1\n"
             "                    geometry\n"
             "                        IndexedLineSet {\n"
             "                            coord Coordinate {\n"
             "                                point [ \n";
  os << Indent ;
  os << CGAL::to_double(t[0].x()) << " " << CGAL::to_double(t[0].y()) 
     << " " << CGAL::to_double(t[0].z()) << ",\n";
  os << Indent;
  os << CGAL::to_double(t[1].x()) << " " << CGAL::to_double(t[1].y()) 
     << " " << CGAL::to_double(t[1].z()) << ",\n";
  os << Indent;
  os << CGAL::to_double(t[2].x()) << " " << CGAL::to_double(t[2].y()) 
     << " " << CGAL::to_double(t[2].z()) << " ]\n";
  os << Indent << "}\n" << Indent << "coordIndex [ 0 1, 1 2, 2 0 -1 ]\n";
  os << Indent << "} # IndexedLineSet\n";
  os << "                } #Shape\n"
        "            ] #children\n"
        "        } #Group\n";
  return os;
}

CGAL_END_NAMESPACE

#endif // CGAL_IO_VRML_2_TRIANGLE_3
#endif // CGAL_TRIANGLE_3_H


#ifdef CGAL_SEGMENT_3_H
#ifndef CGAL_IO_VRML_2_SEGMENT_3
#define CGAL_IO_VRML_2_SEGMENT_3

CGAL_BEGIN_NAMESPACE

template <class R >
VRML_2_ostream&
operator<<(VRML_2_ostream& os,
           const Segment_3<R > &s)
{
  const char *Indent = "                                    ";
  os <<      "        Group {\n"
             "            children [\n"
             "                Shape {\n"
             "                    appearance USE A1\n"
             "                    geometry\n"
             "                        IndexedLineSet {\n"
             "                            coord Coordinate {\n"
             "                                point [ \n";
  os << Indent << CGAL::to_double(s.source().x());
  os << " " << CGAL::to_double(s.source().y()) 
     << " " << CGAL::to_double(s.source().z()) << ",\n";
  os << Indent;
  os << CGAL::to_double(s.target().x()) << " " << CGAL::to_double(s.target().y()) 
     << " " << CGAL::to_double(s.target().z()) << " ]\n";
  os << Indent << "}\n" << Indent << "coordIndex [ 0 1 -1 ]\n";
  os << Indent << "} # IndexedLineSet\n";
  os << "                } #Shape\n"
        "            ] #children\n"
        "        } #Group\n";

  return os;
}

CGAL_END_NAMESPACE

#endif // CGAL_IO_VRML_2_SEGMENT_3
#endif // CGAL_SEGMENT_3_H

#ifdef CGAL_SPHERE_3_H
#ifndef CGAL_IO_VRML_2_SPHERE_3
#define CGAL_IO_VRML_2_SPHERE_3

CGAL_BEGIN_NAMESPACE

template <class R >
VRML_2_ostream&
operator<<(VRML_2_ostream& os,
           const Sphere_3<R > &s)
{
  const char *Indent = "                                    ";
  os <<      "        Group {\n"
             "            children [\n"
             "              Transform {\n"
             "                translation ";
  os <<      CGAL::to_double(s.center().x()) << " " 
     <<      CGAL::to_double(s.center().y()) << " "
     <<      CGAL::to_double(s.center().z()) << "\n";
  os <<      "                children Shape {\n"
             "                    appearance USE A1\n"
             "                    geometry\n"
             "                        Sphere { "
             "radius ";
  os <<      ::sqrt(CGAL::to_double(s.squared_radius())) <<" }\n";
  os <<      "                } #children Shape\n"
             "              } # Transform\n"
             "            ] #children\n"
             "        } #Group\n";

  return os;
}

CGAL_END_NAMESPACE

#endif // CGAL_IO_VRML_2_SEGMENT_3
#endif // CGAL_SEGMENT_3_H


#ifdef CGAL_ALPHA_SHAPE_3_H
#ifndef CGAL_IO_VRML_2_ALPHA_SHAPE_3
#define CGAL_IO_VRML_2_ALPHA_SHAPE_3

CGAL_BEGIN_NAMESPACE

template <class Dt >
VRML_2_ostream&
operator<<(VRML_2_ostream& os,
           Alpha_shape_3<Dt> &as)
{
  // Finite vertices coordinates.
  Alpha_shape_3<Dt>::Alpha_shape_vertices_iterator Vlist_it,
    Vlist_begin = as.alpha_shape_vertices_begin(),
    Vlist_end = as.alpha_shape_vertices_end();

  std::map<Alpha_shape_3<Dt>::Vertex_handle, int> V;
  int number_of_vertex = 0;
  for( Vlist_it = Vlist_begin; Vlist_it != Vlist_end; Vlist_it++) {
    V[*Vlist_it] = number_of_vertex++;
  }

  typename Alpha_shape_3<Dt>::Alpha_shape_facets_iterator Flist_it,
    Flist_begin = as.alpha_shape_facets_begin(),
    Flist_end = as.alpha_shape_facets_end();

  std::map<Alpha_shape_3<Dt>::Facet, int> F;
  int number_of_facets = 0;
  for( Flist_it = Flist_begin; Flist_it != Flist_end; Flist_it++) {
    F[*Flist_it] = number_of_facets++;
  }

  const char *Indent = "                                    ";
  os <<      "        Group {\n"
             "            children [\n"
             "                Shape {\n"
             "                    appearance USE A1\n"
             "                    geometry\n"
             "                        IndexedFaceSet {\n"
             "                            coord Coordinate {\n"
             "                                point [ \n"
     <<      Indent << "  ";
  for( Vlist_it = Vlist_begin; Vlist_it != Vlist_end; Vlist_it++) {
    os << CGAL::to_double((*Vlist_it)->point().x()) << " ";
    os << CGAL::to_double((*Vlist_it)->point().y()) << " ";
    os << CGAL::to_double((*Vlist_it)->point().z()) << ",\n" << Indent << "  ";
  }
    os <<    "\n                                ]\n"
             "                            } # coord\n"
             "                            solid   FALSE\n"
     <<      Indent << "coordIndex  [\n";
  // Finite facets indices.
  for( Flist_it = Flist_begin; Flist_it != Flist_end; Flist_it++){
    os << Indent << "  ";
      for (int i=0; i<4; i++)
        if (i != (*Flist_it).second){
	        os << V[(*Flist_it).first->vertex(i)];
          os << ", ";
        }
    if (Flist_it != Flist_end)
      os << "-1,\n";
    else
      os << "-1 \n";
  }
  os <<      Indent << "]\n";
             "                        } #IndexedFaceSet\n"
             "                } #Shape\n"
             "            ] #children\n"
             "        } #Group\n";

  return os;
}

CGAL_END_NAMESPACE

#endif // CGAL_IO_VRML_2_ALPHA_SHAPE_3
#endif // CGAL_ALPHA_SHAPE_3_H
