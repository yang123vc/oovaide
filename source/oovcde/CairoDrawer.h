/*
 * CairoDrawer.h
 *
 *  Created on: Jul 24, 2013
 *  \copyright 2013 DCBlaha.  Distributed under the GPL.
 */

#ifndef CAIRODRAWER_H_
#define CAIRODRAWER_H_

#include "DiagramDrawer.h"
#include <gtk/gtk.h>

/// Defines a basic context for drawing that can be used for screen or
/// printing to files.
class GtkCairoContext
    {
    public:
	GtkCairoContext(GtkWidget *widget=nullptr):
	    mCr(nullptr)
	    {
	    setContext(widget);
	    }
	void setContext(GtkWidget *widget)
	    {
	    if(widget)
		{
		clear();
		mCr = gdk_cairo_create(gtk_widget_get_window(widget));
		}
	    }
	void clear()
	    {
	    if(mCr)
		cairo_destroy(mCr);
	    }
	~GtkCairoContext()
	    {
	    clear();
	    }
	cairo_t *getCairo()
	    { return mCr; }
	// Pad is currently defined as 1/10 of font size.
//	GraphSize getPad();

    private:
	cairo_t *mCr;
    };

/// Defines a null interface for drawing. This allows drawing to a null device so
/// that calculations can be performed for object sizes based on fonts.
class NullDrawer:public DiagramDrawer
    {
    public:
	NullDrawer(cairo_t *c):
	    cr(c)
	    {}
	void setGraphicsLib(cairo_t *c)
	    { cr = c; }
	bool haveCr() const
	    { return(cr != nullptr); }
	virtual void setDiagramSize(GraphSize /*size*/) override
	    {}
	virtual void drawRect(const GraphRect & /*rect*/) override
	    {}
	virtual void drawLine(const GraphPoint & /*p1*/, const GraphPoint & /*p2*/,
		bool /*dashed=false*/) override
	    {}
	virtual void drawCircle(const GraphPoint & /*p*/, int /*radius*/,
            Color /*fillColor*/) override
	    {}
	virtual void drawEllipse(const GraphRect & /*rect*/) override
	    {}
	virtual void drawPoly(const OovPolygon & /*poly*/, Color /*fillColor*/) override
	    {}
	virtual void drawText(const GraphPoint & /*p*/, OovStringRef const /*text*/) override
	    {}
	virtual float getTextExtentWidth(OovStringRef const /*name*/) const override;
	virtual float getTextExtentHeight(OovStringRef const /*name*/) const override;
    protected:
	cairo_t *cr;
    };

/// This is an interface for drawing using cairo functions.
class CairoDrawer:public NullDrawer
    {
    public:
	CairoDrawer(cairo_t *c):
	    NullDrawer(c)
	    {}
	virtual void setDiagramSize(GraphSize /*size*/) override
	    {}
	virtual void setFontSize(double size) override;
	virtual void drawRect(const GraphRect &rect) override;
	virtual void drawLine(const GraphPoint &p1, const GraphPoint &p2,
		bool dashed=false) override;
	virtual void drawCircle(const GraphPoint &p, int radius, Color fillColor) override;
	virtual void drawEllipse(const GraphRect &rect) override;
	virtual void drawPoly(const OovPolygon &poly, Color fillColor) override;
	virtual void drawText(const GraphPoint &p, OovStringRef const text) override;
	virtual void groupShapes(bool start, Color lineColor, Color fillColor) override;
	virtual void groupText(bool start) override;

	void setColor(Color c)
	    { cairo_set_source_rgb(cr, c.red/255.0, c.green/255.0, c.blue/255.0); }
    private:
	Color mFillColor;
	Color mLineColor;
	Color mTextColor;
    };


#endif /* CAIRODRAWER_H_ */
