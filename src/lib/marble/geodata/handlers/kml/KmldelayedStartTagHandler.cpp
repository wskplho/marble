//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
//

#include "KmldelayedStartTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataSoundCue.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_GX22( delayedStart )

GeoNode* KmldelayedStartTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_delayedStart ) );

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataSoundCue>()) {
        int delay = parser.readElementText().toInt();
        parentItem.nodeAs<GeoDataSoundCue>()->setDelayedStart(delay);
    }

    return 0;
}

} // namespace kml
} // namespace Marble