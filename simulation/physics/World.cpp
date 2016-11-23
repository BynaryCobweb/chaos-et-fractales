//
// Created by louis on 22/10/16.
//
#include <math.h>
#include <iostream>
#include <glm/glm.hpp>

#include "World.h"
#include "Body.h"

World::World() : time(0) {

}

void World::setMethod(const Method &method) {
    this->method = method;
}

void World::addObject(std::shared_ptr<Body> body) {
    this->bodies.push_back(body);
}

vec3d World::getSystemLinearMomentum() {
    vec3d result(0.0, 0.0, 0.0);

    for (auto &body : bodies) {
        vec3d speed(body->vx, body->vy, body->vz);
        result += speed * body->mass;
    }

    return result;
}

vec3d World::getSystemPosition() {
    vec3d result(0.0, 0.0, 0.0);

    for (auto &body : bodies) {
        vec3d position(body->x, body->y, body->z);
        result += position;
    }

    return result;
}

void World::step(double seconds, int increment) {
    time += seconds;

    double sign = seconds > 0 ? 1 : -1;
    seconds = fabs(seconds);
    double timeUnit = fabs(seconds / increment);
    double threshold = timeUnit * 1.5;
    bool simulation = true;

    while (simulation) {
        //Calcul du pas
        if (seconds < threshold) {
            timeUnit = seconds;
            simulation = false;
        }
        else {
            seconds -= timeUnit;
        }

        //CALCUL DES NOUVELLES VARIABLES POUR TOUS LES CORPS
        switch (method) {
        case Method::EULER :
            euler(sign * timeUnit);
            break;
        case Method::RUNGE_KUTTA :
            rungekutta3Bodies(sign * timeUnit);
            break;
        }
    }
}

void World::euler(double t) {

    for (auto &body : this->bodies) {
        body->ax = 0;
        body->ay = 0;
        body->az = 0;

        for (auto &body2 : this->bodies) {
            if (body != body2) {
                double length = body->distance(*body2.get());
                if (length == 0) continue;

                double mg = G * body2->mass;

                body->ax += mg * (body2->x - body->x) / (length * length * length);
                body->ay += mg * (body2->y - body->y) / (length * length * length);
                body->az += mg * (body2->z - body->z) / (length * length * length);
            }
        }

        body->x += body->vx * t + body->ax * t * t;
        body->y += body->vy * t + body->ay * t * t;
        body->z += body->vz * t + body->az * t * t;

        body->vx += body->ax * t;
        body->vy += body->ay * t;
        body->vz += body->az * t;
    }
}


mat3x4d World::rungekuttaFunc3Bodies(mat3x4d & in) {
    mat3x4d out(
            in[0][2], in[0][3], 0, 0,
            in[1][2], in[1][3], 0, 0,
            in[2][2], in[2][3], 0, 0
    );

    for (int i = 0 ; i < 3 ; i++) {
        for (int j = 0 ; j < 3 ; j++) {
            if (i == j) continue;

            double length = bodies[i]->distance(*bodies[j]);
            if (length == 0) continue;

            out[i][2] = out[i][2] + G * bodies[j]->mass * (in[j][0] - in[i][0]) / (length * length * length);
            out[i][3] = out[i][3] + G * bodies[j]->mass * (in[j][1] - in[i][1]) / (length * length * length);
        }
    }

    return out;
}

void World::rungekutta3Bodies(double t) {
    if (bodies.size() != 3) return;

    mat3x4d M(bodies[0]->x, bodies[0]->y, bodies[0]->vx, bodies[0]->vy,
                  bodies[1]->x, bodies[1]->y, bodies[1]->vx, bodies[1]->vy,
                  bodies[2]->x, bodies[2]->y, bodies[2]->vx, bodies[2]->vy);

    auto k1 = rungekuttaFunc3Bodies(M);
    auto M2 = M + k1 * (t / 2);
    auto k2 = rungekuttaFunc3Bodies(M2);
    auto M3 = M + k2 * (t / 2);
    auto k3 = rungekuttaFunc3Bodies(M3);
    auto M4 = M + k3 * t;
    auto k4 = rungekuttaFunc3Bodies(M4);

    bodies[0]->x += (t / 6) * (k1[0][0] + 2 * k2[0][0] + 2 * k3[0][0] + k4[0][0]);
    bodies[0]->y += (t / 6) * (k1[0][1] + 2 * k2[0][1] + 2 * k3[0][1] + k4[0][1]);
    bodies[0]->vx += (t / 6) * (k1[0][2] + 2 * k2[0][2] + 2 * k3[0][2] + k4[0][2]);
    bodies[0]->vy += (t / 6) * (k1[0][3] + 2 * k2[0][3] + 2 * k3[0][3] + k4[0][3]);

    bodies[1]->x += (t / 6) * (k1[1][0] + 2 * k2[1][0] + 2 * k3[1][0] + k4[1][0]);
    bodies[1]->y += (t / 6) * (k1[1][1] + 2 * k2[1][1] + 2 * k3[1][1] + k4[1][1]);
    bodies[1]->vx += (t / 6) * (k1[1][2] + 2 * k2[1][2] + 2 * k3[1][2] + k4[1][2]);
    bodies[1]->vy += (t / 6) * (k1[1][3] + 2 * k2[1][3] + 2 * k3[1][3] + k4[1][3]);

    bodies[2]->x += (t / 6) * (k1[2][0] + 2 * k2[2][0] + 2 * k3[2][0] + k4[2][0]);
    bodies[2]->y += (t / 6) * (k1[2][1] + 2 * k2[2][1] + 2 * k3[2][1] + k4[2][1]);
    bodies[2]->vx += (t / 6) * (k1[2][2] + 2 * k2[2][2] + 2 * k3[2][2] + k4[2][2]);
    bodies[2]->vy += (t / 6) * (k1[2][3] + 2 * k2[2][3] + 2 * k3[2][3] + k4[2][3]);
}