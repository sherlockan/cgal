// Copyright (c) 2008  GeometryFactory Sarl (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#ifndef CGAL_QT_ARRANGEMENT_GRAPHICS_ITEM_H
#define CGAL_QT_ARRANGEMENT_GRAPHICS_ITEM_H

#include <CGAL/Bbox_2.h>
//#include <CGAL/apply_to_range.h>
//#include <CGAL/Kernel/global_functions.h> // TODO: should be included in PainterOstream.h
#include <CGAL/Qt/GraphicsItem.h>
#include <CGAL/Qt/Converter.h>
#include <CGAL/Arr_circular_arc_traits_2.h>
//#include <CGAL/Arr_polyline_traits_2.h>

#include <QGraphicsScene>
#include <QPainter>
//#include <QStyleOption>

#include "ArrangementPainterOstream.h"
#include "Utils.h"
#include <iostream>

class QGraphicsScene;

namespace CGAL {
namespace Qt {

class ArrangementGraphicsItemBase : public GraphicsItem, public QGraphicsSceneMixin
{
public:
    ArrangementGraphicsItemBase( );

    const QPen& getVerticesPen( ) const;
    const QPen& getEdgesPen( ) const;
    void setVerticesPen( const QPen& pen );
    void setEdgesPen( const QPen& pen );
    bool visibleVertices( ) const;
    void setVisibleVertices( const bool b );
    bool visibleEdges( ) const;
    void setVisibleEdges( const bool b );
//    void setScene( QGraphicsScene* scene_ );

protected:
//    QRectF getViewportRect( ) const;

    CGAL::Bbox_2 bb;
    bool bb_initialized;

    QPen verticesPen;
    QPen edgesPen;
    bool visible_edges;
    bool visible_vertices;

    QGraphicsScene* scene;

}; // class ArrangementGraphicsItemBase

template < class Arr_, class ArrTraits = typename Arr_::Geometry_traits_2 >
class ArrangementGraphicsItem : public ArrangementGraphicsItemBase
{
    typedef Arr_ Arrangement;
    typedef typename Arrangement::Geometry_traits_2 Traits;
    typedef typename Arrangement::Vertex_iterator Vertex_iterator;
    typedef typename Arrangement::Curve_iterator Curve_iterator;
    typedef typename Arrangement::Edge_iterator Edge_iterator;
    typedef typename Arrangement::Halfedge Halfedge;
    typedef typename Arrangement::Halfedge_handle Halfedge_handle;
    typedef typename Arrangement::Face_handle Face_handle;
    typedef typename Arrangement::Face_iterator Face_iterator;
    typedef typename Arrangement::Hole_iterator Holes_iterator;
    typedef typename Arrangement::Ccb_halfedge_circulator Ccb_halfedge_circulator;
 
    typedef typename ArrTraitsAdaptor< Traits >::Kernel Kernel;
    typedef typename Traits::X_monotone_curve_2 X_monotone_curve_2;
    typedef typename Kernel::Point_2 Kernel_point_2;
    typedef typename Traits::Point_2 Point_2;
    //typedef typename Kernel::Segment_2 Segment_2;

public:
    ArrangementGraphicsItem( Arrangement* t_ );

public:
    void modelChanged( );
    QRectF boundingRect( ) const;
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

protected:
    template < class TTraits >
    void paint( QPainter* painter, TTraits traits );
    template < class CircularKernel >
    void paint( QPainter* painter, CGAL::Arr_circular_arc_traits_2< CircularKernel > traits );
    template < class Coefficient_ >
    void paint( QPainter* painter, CGAL::Arr_algebraic_segment_traits_2< Coefficient_ > traits );

    void paintFaces( QPainter* painter )
    {
        for( Face_iterator fi = this->arr->faces_begin( ); fi != this->arr->faces_end( ); ++fi )
        {
            fi->set_visited( false );
        }

        Face_handle unboundedFace = this->arr->unbounded_face( );
        this->paintFace( unboundedFace, painter );
    }

#if 0
    template < class TTraits >
    void paintFaces( QPainter* painter, TTraits traits ) { }
    template < class Kernel_ >
    void paintFaces( QPainter* painter, CGAL::Arr_segment_traits_2< Kernel_ > ) { }
    template < class Kernel_ >
    void paintFaces( QPainter* painter, CGAL::Arr_polyline_traits_2< Kernel_ > ) { }
    template < class RatKernel, class AlgKernel, class NtTraits >
    void paintFaces( QPainter* painter, CGAL::Arr_conic_traits_2< RatKernel, AlgKernel, NtTraits > ) { }
    template < class CircularKernel >
    void paintFaces( QPainter* painter, CGAL::Arr_circular_arc_traits_2< CircularKernel > traits ) { }
    template < class Coefficient_ >
    void paintFaces( QPainter* painter, CGAL::Arr_algebraic_segment_traits_2< Coefficient_ > traits ) { }
#endif

    void paintFace( Face_handle f, QPainter* painter )
    {
        if (! f->visited( ) )
        {
            Holes_iterator hit; // holes iterator
            this->paintFace( f, painter, Traits( ) );
            f->set_visited( true );
            for ( hit = f->holes_begin(); hit != f->holes_end(); ++hit )
            {
                Ccb_halfedge_circulator cc = *hit;
                do {
                    Halfedge_handle he = cc;
                    Halfedge_handle he2 = he->twin();
                    Face_handle inner_face = he2->face();
                    if ( this->antenna( he ) )
                        continue;

                    // move on to next hole
                    this->visit_ccb_faces( inner_face, painter );
                } while ( ++cc != *hit );
            }// for
        }
    }

    void visit_ccb_faces( Face_handle & fh, QPainter* painter )
    {
        this->paintFace( fh, painter );

        Ccb_halfedge_circulator cc=fh->outer_ccb();
        do {
            Halfedge he = *cc;
            if (! he.twin()->face()->visited())
            {
                Face_handle nei = (Face_handle) he.twin()->face();
                this->visit_ccb_faces( nei , painter );
            }
            //created from the outer boundary of the face
        } while (++cc != fh->outer_ccb());
    }

    /*! antenna - return true if the halfedge and its
     *  twin point to the same face.
     */
    bool antenna(Halfedge_handle h)
    {
        Halfedge_handle twin = h->twin();
        return (twin->face() == h->face());
    }

    template < class Traits >
    void paintFace( Face_handle f, QPainter* painter, Traits traits ) { }
    template < class Kernel_ >
    void paintFace( Face_handle f, QPainter* painter, CGAL::Arr_segment_traits_2< Kernel_ > )
    {
        if (!f->is_unbounded())  // f is not the unbounded face
        {
            QVector< QPointF > pts; // holds the points of the polygon

            /* running with around the outer of the face and generate from it
             * polygon
             */
            Ccb_halfedge_circulator cc=f->outer_ccb();
            do {
                double x = CGAL::to_double(cc->source()->point().x());
                double y = CGAL::to_double(cc->source()->point().y());
                QPointF coord_source(x , y);
                pts.push_back(coord_source );
                //created from the outer boundary of the face
            } while (++cc != f->outer_ccb());

            // make polygon from the outer ccb of the face 'f'
            QPolygonF pgn (pts);

            // FIXME: get the bg color
            QColor color = ::Qt::black;
            if ( f->color().isValid() )
            {
                color = f->color();
            }
            QBrush oldBrush = painter->brush( );
            painter->setBrush( color );
            painter->drawPolygon( pgn );
            painter->setBrush( oldBrush );
        }
        else
        {
            QRectF rect = this->viewportRect( );
            QColor color = ::Qt::black;
            painter->fillRect( rect, color );
        }
    }

    //void cacheCurveBoundingRects( );
    void updateBoundingBox( );
    template < class TTraits >
    void updateBoundingBox( TTraits traits );
    template < class Coefficient_ >
    void updateBoundingBox( CGAL::Arr_algebraic_segment_traits_2< Coefficient_ > traits );

    Arrangement* arr;
    ArrangementPainterOstream< Traits > painterostream;
    CGAL::Qt::Converter< Kernel > convert;
    std::map< Curve_iterator, CGAL::Bbox_2 > curveBboxMap;
}; // class ArrangementGraphicsItem

template < class Arr_, class ArrTraits >
ArrangementGraphicsItem< Arr_, ArrTraits >::
ArrangementGraphicsItem( Arrangement* arr_ ):
    arr( arr_ ),
    painterostream( 0 )
{
    if ( this->arr->number_of_vertices( ) == 0 ) {
        this->hide( );
    }
    this->updateBoundingBox( );
    this->setZValue( 3 );
}

template < class Arr_, class ArrTraits >
QRectF 
ArrangementGraphicsItem< Arr_, ArrTraits >::
boundingRect( ) const
{
    QRectF rect = this->convert( this->bb );
    return rect;
}

template < class Arr_, class ArrTraits >
void 
ArrangementGraphicsItem< Arr_, ArrTraits >::paint(QPainter *painter, 
                                    const QStyleOptionGraphicsItem *option,
                                    QWidget * /*widget*/)
{
    this->paint( painter, ArrTraits( ) );
}

template < class Arr_, class ArrTraits >
template < class TTraits >
void
ArrangementGraphicsItem< Arr_, ArrTraits >::
paint( QPainter* painter, TTraits traits )
{
    this->paintFaces( painter );

    painter->setPen( this->verticesPen );
    this->painterostream = ArrangementPainterOstream< Traits >( painter, this->boundingRect( ) );
    this->painterostream.setScene( this->scene );

    for ( Vertex_iterator it = this->arr->vertices_begin( ); it != this->arr->vertices_end( ); ++it )
    {
        Point_2 p = it->point( );
        Kernel_point_2 pt( p.x( ), p.y( ) );
        this->painterostream << pt;
    }
    painter->setPen( this->edgesPen );
    for ( Edge_iterator it = this->arr->edges_begin( ); it != this->arr->edges_end( ); ++it )
    {
        X_monotone_curve_2 curve = it->curve( );
        this->painterostream << curve;
    }
}

template < class Arr_, class ArrTraits >
template < class CircularKernel >
void
ArrangementGraphicsItem< Arr_, ArrTraits >::
paint( QPainter* painter, CGAL::Arr_circular_arc_traits_2< CircularKernel > traits )
{
    typedef Kernel_point_2 Non_arc_point_2;
    typedef typename Traits::Point_2 Arc_point_2;

    painter->setPen( this->verticesPen );
    this->painterostream = ArrangementPainterOstream< Traits >( painter, this->boundingRect( ) );
    this->painterostream.setScene( this->scene );

    for ( Vertex_iterator it = this->arr->vertices_begin( ); it != this->arr->vertices_end( ); ++it )
    {
        Arc_point_2 pt = it->point( );
        Non_arc_point_2 pt2(CGAL::to_double(pt.x( )), CGAL::to_double(pt.y()) );
        this->painterostream << pt2;
    }
    painter->setPen( this->edgesPen );
    for ( Edge_iterator it = this->arr->edges_begin( ); it != this->arr->edges_end( ); ++it )
    {
        X_monotone_curve_2 curve = it->curve( );
        this->painterostream << curve;
    }
}

template < class Arr_, class ArrTraits >
template < class Coefficient_ >
void
ArrangementGraphicsItem< Arr_, ArrTraits >::
paint( QPainter* painter, CGAL::Arr_algebraic_segment_traits_2< Coefficient_ > traits )
{
    painter->setPen( this->verticesPen );
    QRectF clipRect = this->boundingRect( );
    if ( std::isinf(clipRect.left( )) || 
        std::isinf(clipRect.right( )) || 
        std::isinf(clipRect.top( )) || 
        std::isinf(clipRect.bottom( )) )
    {
        clipRect = this->viewportRect( );
    }

    this->painterostream = ArrangementPainterOstream< Traits >( painter, clipRect );
    this->painterostream.setScene( this->scene );

    for ( Vertex_iterator it = this->arr->vertices_begin( ); it != this->arr->vertices_end( ); ++it )
    {
        Point_2 p = it->point( );
        //std::pair< double, double > approx = p.to_double( );
        //Kernel_point_2 pt( approx.first, approx.second );
        //this->painterostream << pt;
        this->painterostream << p;
    }
    painter->setPen( this->edgesPen );
    for ( Edge_iterator it = this->arr->edges_begin( ); it != this->arr->edges_end( ); ++it )
    {
        X_monotone_curve_2 curve = it->curve( );
        this->painterostream << curve;
    }
}

// We let the bounding box only grow, so that when vertices get removed
// the maximal bbox gets refreshed in the GraphicsView
template < class Arr_, class ArrTraits >
void 
ArrangementGraphicsItem< Arr_, ArrTraits >::updateBoundingBox( )
{
    this->updateBoundingBox( ArrTraits( ) );
}

template < class Arr_, class ArrTraits >
template < class TTraits >
void 
ArrangementGraphicsItem< Arr_, ArrTraits >::updateBoundingBox( TTraits traits )
{
    this->prepareGeometryChange( );
    if ( this->arr->number_of_vertices( ) == 0 )
    {
        this->bb = Bbox_2( 0, 0, 0, 0 );
        this->bb_initialized = false;
        return;
    }
    else
    {
        this->bb = this->arr->vertices_begin( )->point( ).bbox( );
        this->bb_initialized = true;
    }

    for ( Curve_iterator it = this->arr->curves_begin( );
        it != this->arr->curves_end( );
        ++it )
    {
        if ( this->curveBboxMap.count( it ) == 0 )
        {
            this->curveBboxMap[ it ] = it->bbox( );
        }
        this->bb = this->bb + this->curveBboxMap[ it ];
    }
}

template < class Arr_, class ArrTraits >
template < class Coefficient_ >
void
ArrangementGraphicsItem< Arr_, ArrTraits >::
updateBoundingBox( CGAL::Arr_algebraic_segment_traits_2< Coefficient_ > traits )
{
    this->prepareGeometryChange( );
    if ( this->arr->number_of_vertices( ) == 0 )
    {
        this->bb = Bbox_2( 0, 0, 0, 0 );
        this->bb_initialized = false;
        return;
    }
    else
    {
        //std::pair< double, double > approx = this->arr->vertices_begin( )->point( ).to_double( );
        //this->bb = CGAL::Bbox_2( approx.first, approx.second, approx.first, approx.second );
        this->bb = CGAL::Bbox_2( 0, 0, 0, 0 );
        this->bb_initialized = true;
    }
    typename Traits::Make_x_monotone_2 make_x_monotone_2 = traits.make_x_monotone_2_object( );
    for ( Curve_iterator it = this->arr->curves_begin( );
        it != this->arr->curves_end( );
        ++it )
    {
        std::vector< CGAL::Object > cvs;
        make_x_monotone_2( *it, std::back_inserter( cvs ) );
        for ( int i = 0 ; i < cvs.size( ); ++i )
        {
            X_monotone_curve_2 cv;
            CGAL::assign( cv, cvs[ i ] );
            this->bb = this->bb + cv.bbox( );
        }
    }
}

template < class Arr_, class ArrTraits >
void 
ArrangementGraphicsItem< Arr_, ArrTraits >::modelChanged( )
{
    if ( this->arr->is_empty( ) )
    {
        this->hide( );
    }
    else
    {
        this->show( );
    }
    this->updateBoundingBox( );
    this->update( );
}

/**
Specialized methods:
    updateBoundingBox
*/
template < class Arr_, class Kernel_ >
class ArrangementGraphicsItem< Arr_, CGAL::Arr_linear_traits_2< Kernel_ > >  : public ArrangementGraphicsItemBase
{
    typedef Arr_ Arrangement;
    typedef ArrangementGraphicsItemBase Superclass;
    typedef typename Arrangement::Geometry_traits_2 Traits;
    typedef typename Arrangement::Vertex_iterator Vertex_iterator;
    typedef typename Arrangement::Curve_iterator Curve_iterator;
    typedef typename Arrangement::Edge_iterator Edge_iterator;
    typedef typename ArrTraitsAdaptor< Traits >::Kernel Kernel;
    typedef typename Traits::X_monotone_curve_2 X_monotone_curve_2;
    typedef typename Kernel::Point_2 Point_2;
    typedef typename Kernel::Segment_2 Segment_2;

public:
    ArrangementGraphicsItem( Arrangement* arr_ ):
        arr( arr_ ),
        painterostream( 0 )
    {
        if ( this->arr->number_of_vertices( ) == 0 ) {
            this->hide( );
        }
        this->updateBoundingBox( );
        this->setZValue( 3 );
    }

    void modelChanged( )
    {
        if ( this->arr->is_empty( ) )
        {
            this->hide( );
        }
        else
        {
            this->show( );
        }
        this->updateBoundingBox( );
        this->update( );
    }


public: // methods
    // @override QGraphicsItem::boundingRect
    QRectF boundingRect( ) const
    {
        QRectF rect = this->convert( this->bb );
        return rect;
    }

    // @override QGraphicsItem::paint
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
    {
        this->updateBoundingBox( );
        painter->setPen( this->verticesPen );
        this->painterostream = ArrangementPainterOstream< Traits >( painter, this->boundingRect( ) );
        this->painterostream.setScene( this->scene );

        for ( Vertex_iterator it = this->arr->vertices_begin( ); it != this->arr->vertices_end( ); ++it )
        {
            Point_2 pt = it->point( );
            this->painterostream << pt;
        }
        painter->setPen( this->edgesPen );
        for ( Edge_iterator it = this->arr->edges_begin( ); it != this->arr->edges_end( ); ++it )
        {
            X_monotone_curve_2 curve = it->curve( );
            this->painterostream << curve;
        }
    }

protected: // methods
    void updateBoundingBox( )
    {
        this->prepareGeometryChange( );
        QRectF clipRect = this->viewportRect( );
        this->convert = Converter<Kernel>( clipRect );

        if ( ! clipRect.isValid( ) /*|| this->arr->number_of_vertices( ) == 0*/ )
        {
            this->bb = Bbox_2( 0, 0, 0, 0 );
            this->bb_initialized = false;
            return;
        }
        else
        {
            this->bb = this->convert( clipRect ).bbox( );
            this->bb_initialized = true;
        }

        for ( Curve_iterator it = this->arr->curves_begin( );
            it != this->arr->curves_end( );
            ++it )
        {
            if ( it->is_segment( ) )
            {
                this->bb = this->bb + it->segment( ).bbox( );
            }
            else if ( it->is_ray( ) )
            {
                QLineF qclippedRay = this->convert( it->ray( ) );
                Segment_2 clippedRay = this->convert( qclippedRay );
                this->bb = this->bb + clippedRay.bbox( );
            }
            else // ( it->is_line( ) )
            {
                QLineF qclippedLine = this->convert( it->line( ) );
                Segment_2 clippedLine = this->convert( qclippedLine );
                this->bb = this->bb + clippedLine.bbox( );
            }
        }
    }

protected: // fields
    Arrangement* arr;
    ArrangementPainterOstream< Traits > painterostream;
    CGAL::Qt::Converter< Kernel > convert;
}; // class ArrangementGraphicsItem

} // namespace Qt
} // namespace CGAL

#endif // CGAL_QT_ARRANGEMENT_GRAPHICS_ITEM_H
