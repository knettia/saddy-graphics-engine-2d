#include "animations/setstate/setcamerarotation.h"
#include "camera.h"
#include "opticksupport.h"

sad::animations::setstate::SetCameraRotation::SetCameraRotation(sad::Scene* s, const sad::Point3D& p) : m_scene(s), m_pivot(p)
{
    PROFILER_EVENT;

}

sad::animations::setstate::AbstractSetStateCommand* 
sad::animations::setstate::SetCameraRotation::clone() const
{
    PROFILER_EVENT;
    return new sad::animations::setstate::SetCameraRotation(m_scene, m_pivot);
}

void sad::animations::setstate::SetCameraRotation::call(const double& angle)
{
    PROFILER_EVENT;
    sad::Camera& c = m_scene->camera();
    c.setAngle(angle);
    c.setRotationVectorDirection(sad::Vector3D(0, 0, 1));
    c.setTemporaryRotationOffset(m_pivot);
}

sad::animations::setstate::SetCameraRotation::~SetCameraRotation()
{
    PROFILER_EVENT;

}
