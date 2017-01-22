#pragma once

#include <QObject>
#include <QtTest/QtTest>

#include <memory>

#include "spline_library/vector.h"
#include "spline_library/spline.h"

Q_DECLARE_METATYPE(Vector2)
Q_DECLARE_METATYPE(Vector3)
Q_DECLARE_METATYPE(std::shared_ptr<Spline<Vector2>>)


//perform a linear interpolation between a and b
template<class InterpolationType, typename floating_t>
InterpolationType lerp(InterpolationType a, InterpolationType b, floating_t t)
{
    return a * (floating_t(1) - t) + b * t;
}

//we need to pad out the ends of the data differently depending on spline type
//this way all of the splines will have the same arc length, so it'll be easier to test
template<class T>
std::vector<T> addPadding(std::vector<T> list, size_t paddingSize)
{
    list.reserve(list.size() + paddingSize * 2);
    for(size_t i = 0; i < paddingSize; i++) {
        list.insert(list.begin(), list[0] - (list[1] - list[0]));
    }
    for(size_t i = 0; i < paddingSize; i++) {
        list.push_back(list[list.size() - 1] + (list[list.size() - 1] - list[list.size() - 2]));
    }
    return list;
}

//Given a list of points, compute an equal-sized list of tangents to use in a cubic or quintic hermite spline
//use the finite difference algorithm
template<class T>
std::vector<T> makeTangents(std::vector<T> points)
{
    std::vector<T> tangents(points.size());

    //one-sided difference at the start
    tangents[0] = points[1] - points[0];

    //normal finite difference in the middle
    for(size_t i = 1; i < points.size() - 1; i++)
    {
        tangents[i] = 0.5f * ((points[i+1] - points[i]) + (points[i] - points[i-1]));
    }

    //one-sided difference at the start
    tangents[tangents.size() - 1] = points[points.size() - 1] - points[points.size() - 2];
    return tangents;
}

template<class T>
void compareFloatsLenient(T actual, T expected, T tol)
{
    auto error = std::abs(actual - expected) / expected;
    if(error > tol) {
        std::string errorMessage = QString("Compared floats were different. Actual: %1, Expected: %2").arg(QString::number(actual), QString::number(expected)).toStdString();
        QFAIL(errorMessage.data());
    }
};
