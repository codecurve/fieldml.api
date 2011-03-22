/* \file
 * $Id$
 * \author Caton Little
 * \brief 
 *
 * \section LICENSE
 *
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the \"License\"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an \"AS IS\"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is FieldML
 *
 * The Initial Developer of the Original Code is Auckland Uniservices Ltd,
 * Auckland, New Zealand. Portions created by the Initial Developer are
 * Copyright (C) 2010 the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the \"GPL\"), or
 * the GNU Lesser General Public License Version 2.1 or later (the \"LGPL\"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 */

#include "fieldml_library_0.3.h"

const char * const FML_LIBRARY_0_3_STRING = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> \
<fieldml version=\"0.3_alpha\" xsi:noNamespaceSchemaLocation=\"Fieldml_0.3.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"> \
 <Region name=\"library\"> \
  <ContinuousType name=\"library.real.1d\"/> \
  <AbstractEvaluator name=\"library.real.1d.variable\" valueType=\"library.real.1d\"/> \
  <EnsembleType name=\"library.ensemble.generic.2d\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"2\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.ensemble.generic.2d.variable\" valueType=\"library.ensemble.generic.2d\"/> \
  <ContinuousType name=\"library.real.2d\" componentEnsemble=\"library.ensemble.generic.2d\"/> \
  <AbstractEvaluator name=\"library.real.2d.variable\" valueType=\"library.real.2d\"/> \
  <EnsembleType name=\"library.ensemble.generic.3d\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"3\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.ensemble.generic.3d.variable\" valueType=\"library.ensemble.generic.3d\"/> \
  <ContinuousType name=\"library.real.3d\" componentEnsemble=\"library.ensemble.generic.3d\"/> \
  <AbstractEvaluator name=\"library.real.3d.variable\" valueType=\"library.real.3d\"/> \
  <EnsembleType name=\"library.ensemble.xi.1d\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"1\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.ensemble.xi.1d.variable\" valueType=\"library.ensemble.xi.1d\"/> \
  <ContinuousType name=\"library.xi.1d\" componentEnsemble=\"library.ensemble.xi.1d\"/> \
  <AbstractEvaluator name=\"library.xi.1d.variable\" valueType=\"library.xi.1d\"/> \
  <EnsembleType name=\"library.ensemble.xi.2d\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"2\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.ensemble.xi.2d.variable\" valueType=\"library.ensemble.xi.2d\"/> \
  <ContinuousType name=\"library.xi.2d\" componentEnsemble=\"library.ensemble.xi.2d\"/> \
  <AbstractEvaluator name=\"library.xi.2d.variable\" valueType=\"library.xi.2d\"/> \
  <EnsembleType name=\"library.ensemble.xi.3d\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"3\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.ensemble.xi.3d.variable\" valueType=\"library.ensemble.xi.3d\"/> \
  <ContinuousType name=\"library.xi.3d\" componentEnsemble=\"library.ensemble.xi.3d\"/> \
  <AbstractEvaluator name=\"library.xi.3d.variable\" valueType=\"library.xi.3d\"/> \
  <EnsembleType name=\"library.localNodes.1d.line2\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"2\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.localNodes.1d.line2.variable\" valueType=\"library.localNodes.1d.line2\"/> \
  <ContinuousType name=\"library.parameters.1d.linearLagrange\" componentEnsemble=\"library.localNodes.1d.line2\"/> \
  <AbstractEvaluator name=\"library.parameters.1d.linearLagrange.variable\" valueType=\"library.parameters.1d.linearLagrange\"/> \
 \
  <ExternalEvaluator name=\"library.interpolator.1d.unit.linearLagrange\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.1d.variable\"/> \
      <variable name=\"library.parameters.1d.linearLagrange.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <EnsembleType name=\"library.localNodes.1d.line3\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"3\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.localNodes.1d.line3.variable\" valueType=\"library.localNodes.1d.line3\"/> \
  <ContinuousType name=\"library.parameters.1d.quadraticLagrange\" componentEnsemble=\"library.localNodes.1d.line3\"/> \
  <AbstractEvaluator name=\"library.parameters.1d.quadraticLagrange.variable\" valueType=\"library.parameters.1d.quadraticLagrange\"/> \
 \
  <ExternalEvaluator name=\"library.interpolator.1d.unit.quadraticLagrange\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.1d.variable\"/> \
      <variable name=\"library.parameters.1d.quadraticLagrange.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <EnsembleType name=\"library.localNodes.2d.square2x2\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"4\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.localNodes.2d.square2x2.variable\" valueType=\"library.localNodes.2d.square2x2\"/> \
  <ContinuousType name=\"library.parameters.2d.bilinearLagrange\" componentEnsemble=\"library.localNodes.2d.square2x2\"/> \
  <AbstractEvaluator name=\"library.parameters.2d.bilinearLagrange.variable\" valueType=\"library.parameters.2d.bilinearLagrange\"/> \
 \
  <ExternalEvaluator name=\"library.interpolator.2d.unit.bilinearLagrange\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.2d.variable\"/> \
      <variable name=\"library.parameters.2d.bilinearLagrange.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <EnsembleType name=\"library.localNodes.2d.square3x3\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"9\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.localNodes.2d.square3x3.variable\" valueType=\"library.localNodes.2d.square3x3\"/> \
  <ContinuousType name=\"library.parameters.2d.biquadraticLagrange\" componentEnsemble=\"library.localNodes.2d.square3x3\"/> \
  <AbstractEvaluator name=\"library.parameters.2d.biquadraticLagrange.variable\" valueType=\"library.parameters.2d.biquadraticLagrange\"/> \
 \
  <ExternalEvaluator name=\"library.interpolator.2d.unit.biquadraticLagrange\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.2d.variable\"/> \
      <variable name=\"library.parameters.2d.biquadraticLagrange.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <EnsembleType name=\"library.localNodes.3d.cube2x2x2\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"8\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.localNodes.3d.cube2x2x2.variable\" valueType=\"library.localNodes.3d.cube2x2x2\"/> \
  <ContinuousType name=\"library.parameters.3d.trilinearLagrange\" componentEnsemble=\"library.localNodes.3d.cube2x2x2\"/> \
  <AbstractEvaluator name=\"library.parameters.3d.trilinearLagrange.variable\" valueType=\"library.parameters.3d.trilinearLagrange\"/> \
 \
  <ExternalEvaluator name=\"library.interpolator.3d.unit.trilinearLagrange\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.3d.variable\"/> \
      <variable name=\"library.parameters.3d.trilinearLagrange.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <EnsembleType name=\"library.localNodes.3d.cube3x3x3\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"27\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.localNodes.3d.cube3x3x3.variable\" valueType=\"library.localNodes.3d.cube3x3x3\"/> \
  <ContinuousType name=\"library.parameters.3d.triquadraticLagrange\" componentEnsemble=\"library.localNodes.3d.cube3x3x3\"/> \
  <AbstractEvaluator name=\"library.parameters.3d.triquadraticLagrange.variable\" valueType=\"library.parameters.3d.triquadraticLagrange\"/> \
 \
  <ExternalEvaluator name=\"library.interpolator.3d.unit.triquadraticLagrange\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.3d.variable\"/> \
      <variable name=\"library.parameters.3d.triquadraticLagrange.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <ContinuousType name=\"library.coordinates.rc.1d\"/> \
  <AbstractEvaluator name=\"library.coordinates.rc.1d.variable\" valueType=\"library.coordinates.rc.1d\"/> \
 \
  <EnsembleType name=\"library.ensemble.rc.2d\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"2\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.ensemble.rc.2d.variable\" valueType=\"library.ensemble.rc.2d\"/> \
  <ContinuousType name=\"library.coordinates.rc.2d\" componentEnsemble=\"library.ensemble.rc.2d\"/> \
  <AbstractEvaluator name=\"library.coordinates.rc.2d.variable\" valueType=\"library.coordinates.rc.2d\"/> \
 \
  <EnsembleType name=\"library.ensemble.rc.3d\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"3\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.ensemble.rc.3d.variable\" valueType=\"library.ensemble.rc.3d\"/> \
  <ContinuousType name=\"library.coordinates.rc.3d\" componentEnsemble=\"library.ensemble.rc.3d\"/> \
  <AbstractEvaluator name=\"library.coordinates.rc.3d.variable\" valueType=\"library.coordinates.rc.3d\"/> \
 \
  <EnsembleType name=\"library.parameters.1d.cubicHermite.ensemble\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"4\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.parameters.1d.cubicHermite.ensemble.variable\" valueType=\"library.parameters.1d.cubicHermite.ensemble\"/> \
  <ContinuousType name=\"library.parameters.1d.cubicHermite\" componentEnsemble=\"library.parameters.1d.cubicHermite.ensemble\"/> \
  <AbstractEvaluator name=\"library.parameters.1d.cubicHermite.variable\" valueType=\"library.parameters.1d.cubicHermite\"/> \
  <AbstractEvaluator name=\"library.parameters.1d.cubicHermiteScaling.variable\" valueType=\"library.parameters.1d.cubicHermite\"/> \
 \
  <ExternalEvaluator name=\"library.interpolator.1d.unit.cubicHermite\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.1d.variable\"/> \
      <variable name=\"library.parameters.1d.cubicHermite.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <ExternalEvaluator name=\"library.interpolator.1d.unit.cubicHermiteScaled\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.1d.variable\"/> \
      <variable name=\"library.parameters.1d.cubicHermite.variable\"/> \
      <variable name=\"library.parameters.1d.cubicHermiteScaling.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <EnsembleType name=\"library.parameters.2d.bicubicHermite.ensemble\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"16\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.parameters.2d.bicubicHermite.ensemble.variable\" valueType=\"library.parameters.2d.bicubicHermite.ensemble\"/> \
  <ContinuousType name=\"library.parameters.2d.bicubicHermite\" componentEnsemble=\"library.parameters.2d.bicubicHermite.ensemble\"/> \
  <AbstractEvaluator name=\"library.parameters.2d.bicubicHermite.variable\" valueType=\"library.parameters.2d.bicubicHermite\"/> \
  <AbstractEvaluator name=\"library.parameters.2d.bicubicHermiteScaling.variable\" valueType=\"library.parameters.2d.bicubicHermite\"/> \
 \
  <ExternalEvaluator name=\"library.interpolator.2d.unit.bicubicHermite\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.2d.variable\"/> \
      <variable name=\"library.parameters.2d.bicubicHermite.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <ExternalEvaluator name=\"library.interpolator.2d.unit.bicubicHermiteScaled\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.2d.variable\"/> \
      <variable name=\"library.parameters.2d.bicubicHermite.variable\"/> \
      <variable name=\"library.parameters.2d.bicubicHermiteScaling.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <EnsembleType name=\"library.parameters.3d.tricubicHermite.ensemble\" isComponentEnsemble=\"true\"> \
   <members> \
    <member_range min=\"1\" max=\"64\"/> \
   </members> \
  </EnsembleType> \
  <AbstractEvaluator name=\"library.parameters.3d.tricubicHermite.ensemble.variable\" valueType=\"library.parameters.3d.tricubicHermite.ensemble\"/> \
  <ContinuousType name=\"library.parameters.3d.tricubicHermite\" componentEnsemble=\"library.parameters.3d.tricubicHermite.ensemble\"/> \
  <AbstractEvaluator name=\"library.parameters.3d.tricubicHermite.variable\" valueType=\"library.parameters.3d.tricubicHermite\"/> \
  <AbstractEvaluator name=\"library.parameters.3d.tricubicHermiteScaling.variable\" valueType=\"library.parameters.3d.tricubicHermite\"/> \
 \
  <ExternalEvaluator name=\"library.interpolator.3d.unit.tricubicHermite\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.3d.variable\"/> \
      <variable name=\"library.parameters.3d.tricubicHermite.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
  <ExternalEvaluator name=\"library.interpolator.3d.unit.tricubicHermiteScaled\" valueType=\"library.real.1d\"> \
    <variables> \
      <variable name=\"library.xi.3d.variable\"/> \
      <variable name=\"library.parameters.3d.tricubicHermite.variable\"/> \
      <variable name=\"library.parameters.3d.tricubicHermiteScaling.variable\"/> \
    </variables> \
  </ExternalEvaluator> \
 \
 </Region> \
</fieldml> \
";
