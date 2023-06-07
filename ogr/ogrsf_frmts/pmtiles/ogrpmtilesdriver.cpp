/******************************************************************************
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implementation of PMTiles
 * Author:   Even Rouault <even.rouault at spatialys.com>
 *
 ******************************************************************************
 * Copyright (c) 2023, Planet Labs
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "ogr_pmtiles.h"

#include "vsipmtiles.h"

/************************************************************************/
/*                     OGRPMTilesDriverIdentify()                       */
/************************************************************************/

static int OGRPMTilesDriverIdentify(GDALOpenInfo *poOpenInfo)
{
    if (poOpenInfo->nHeaderBytes < 127 || !poOpenInfo->fpL)
        return FALSE;
    return memcmp(poOpenInfo->pabyHeader, "PMTiles\x03", 8) == 0;
}

/************************************************************************/
/*                       OGRPMTilesDriverOpen()                         */
/************************************************************************/

static GDALDataset *OGRPMTilesDriverOpen(GDALOpenInfo *poOpenInfo)
{
    if (!OGRPMTilesDriverIdentify(poOpenInfo))
        return nullptr;
    auto poDS = cpl::make_unique<OGRPMTilesDataset>();
    if (!poDS->Open(poOpenInfo))
        return nullptr;
    return poDS.release();
}

/************************************************************************/
/*                          RegisterOGRPMTiles()                        */
/************************************************************************/

void RegisterOGRPMTiles()
{
    if (GDALGetDriverByName("PMTiles") != nullptr)
        return;

    VSIPMTilesRegister();

    GDALDriver *poDriver = new GDALDriver();

    poDriver->SetDescription("PMTiles");
    poDriver->SetMetadataItem(GDAL_DCAP_VECTOR, "YES");
    poDriver->SetMetadataItem(GDAL_DMD_LONGNAME, "ProtoMap Tiles");
    poDriver->SetMetadataItem(GDAL_DMD_EXTENSIONS, "pmtiles");
    poDriver->SetMetadataItem(GDAL_DMD_HELPTOPIC,
                              "drivers/vector/pmtiles.html");

    poDriver->SetMetadataItem(
        GDAL_DMD_OPENOPTIONLIST,
        "<OpenOptionList>"
        "  <Option name='ZOOM_LEVEL' type='integer' "
        "description='Zoom level of full resolution. If not specified, maximum "
        "non-empty zoom level'/>"
        "  <Option name='CLIP' type='boolean' "
        "description='Whether to clip geometries to tile extent' "
        "default='YES'/>"
        "  <Option name='ZOOM_LEVEL_AUTO' type='boolean' "
        "description='Whether to auto-select the zoom level for vector layers "
        "according to spatial filter extent. Only for display purpose' "
        "default='NO'/>"
        "  <Option name='JSON_FIELD' type='boolean' "
        "description='For vector layers, "
        "whether to put all attributes as a serialized JSon dictionary'/>"
        "</OpenOptionList>");

    poDriver->SetMetadataItem(GDAL_DCAP_VIRTUALIO, "YES");

    poDriver->pfnOpen = OGRPMTilesDriverOpen;
    poDriver->pfnIdentify = OGRPMTilesDriverIdentify;

    GetGDALDriverManager()->RegisterDriver(poDriver);
}
