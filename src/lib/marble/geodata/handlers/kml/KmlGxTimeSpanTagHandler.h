//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#ifndef KMLGXTIMESPANTAGHANDLER_H
#define KMLGXTIMESPANTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{
namespace gx
{

class KmlTimeSpanTagHandler : public GeoTagHandler
{
public:
    virtual GeoNode* parse(GeoParser&) const;
};

}
}
}

#endif // KMLGXTIMESPANTAGHANDLER_H
