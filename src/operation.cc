// Copyright (C) 2015, Romain Goffe <romain.goffe@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//******************************************************************************

#include "operation.hh"

Operation::Operation() :
  m_name(),
  m_description(),
  m_url()
{
}

Operation::Operation(const Operation & p_other) :
  m_name(p_other.name()),
  m_description(p_other.description()),
  m_url(p_other.url())
{
}

Operation::~Operation()
{
}

const QString & Operation::name() const
{
  return m_name;
}

void Operation::setName(const QString & p_name)
{
  m_name = p_name;
}

const QString & Operation::description() const
{
  return m_description;
}

void Operation::setDescription(const QString & p_description)
{
  m_description = p_description;
}

const QUrl & Operation::url() const
{
  return m_url;
}

void Operation::setUrl(const QUrl & p_url)
{
  m_url = p_url;
}

void Operation::setUrl(const QString & p_url)
{
  m_url = QUrl(p_url);
}


QList<Operation> Operation::list_benchmark()
{
  QList<Operation> list;
  Operation o;

  o.setName("total");
  o.setDescription("Number of elements");
  o.setUrl("http://docs.opencv.org/modules/core/doc/dynamic_structures.html#int%20total");
  list << Operation(o);

  o.setName("countNonZeros");
  o.setDescription("Number of non-zero elements");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#countnonzero");
  list << Operation(o);

  o.setName("minMaxLoc");
  o.setDescription("Min/Max values and positions");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#void%20minMaxLoc%28InputArray%20src,%20double*%20minVal,%20double*%20maxVal,%20Point*%20minLoc,%20Point*%20maxLoc,%20InputArray%20mask%29");
  list << Operation(o);

  o.setName("meanStdDev");
  o.setDescription("Compute mean and standard deviation");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#void%20meanStdDev%28InputArray%20src,%20OutputArray%20mean,%20OutputArray%20stddev,%20InputArray%20mask%29");
  list << Operation(o);

  o.setName("add");
  o.setDescription("Scalar addition");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#add");
  list << Operation(o);

  o.setName("multiply");
  o.setDescription("Scalar multiplication");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#multiply");
  list << Operation(o);

  o.setName("transpose");
  o.setDescription("Matrix transposition");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#transpose");
  list << Operation(o);

  o.setName("verticalFlip");
  o.setDescription("Flip along x axis");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#flip");
  list << Operation(o);

  o.setName("horizontalFlip");
  o.setDescription("Flip along y axis");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#flip");
  list << Operation(o);

  o.setName("rotate");
  o.setDescription("Matrix 2d rotation");
  o.setUrl("http://docs.opencv.org/modules/imgproc/doc/geometric_transformations.html#Mat%20getRotationMatrix2D%28Point2f%20center,%20double%20angle,%20double%20scale%29");
  list << Operation(o);

  o.setName("normalize");
  o.setDescription("Matrix normalization");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#normalize");
  list << Operation(o);

  o.setName("mulTranspose");
  o.setDescription("Multiplication by transposed (m * m.t())");
  o.setUrl("http://docs.opencv.org/modules/core/doc/operations_on_arrays.html#void%20mulTransposed%28InputArray%20src,%20OutputArray%20dst,%20bool%20aTa,%20InputArray%20delta,%20double%20scale,%20int%20dtype%29");
  list << Operation(o);

  return list;
}
