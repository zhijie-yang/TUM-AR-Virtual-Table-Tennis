#include "libtennis/ball.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

const float & libtennis::ball::get_radius() const
{
    return _radius;
}

const float & libtennis::ball::get_mass() const
{
    return _mass;
}

const glm::vec3& libtennis::ball::get_position() const
{
	return _position;
}

void libtennis::ball::set_position(const glm::vec3& value)
{
	_position = value;
}

const glm::vec3& libtennis::ball::get_velocity() const
{
	return _velocity;
}

void libtennis::ball::set_velocity(const glm::vec3& value)
{
	_velocity = value;
}

size_t libtennis::ball::get_serialize_size()
{
	return size_t(6 * sizeof(glm::vec3::value_type));
}

void libtennis::ball::serialize(unsigned char* buffer)
{
	auto* values = reinterpret_cast<glm::vec3::value_type*>(buffer);
	
	values[0] = _position.x;
	values[1] = _position.y;
	values[2] = _position.z;

	values[3] = _velocity.x;
	values[4] = _velocity.y;
	values[5] = _velocity.z;
}

glm::mat4 libtennis::ball::get_model() const
{
	return glm::translate(glm::mat4(1.0f), _position);
}
