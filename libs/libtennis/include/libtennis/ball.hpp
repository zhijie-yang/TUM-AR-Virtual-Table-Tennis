#ifndef LIBTENNIS_BALL
#define LIBTENNIS_BALL

#include <glm/glm.hpp>

namespace libtennis {
	class ball {
	private:
		glm::vec3 _position;
		glm::vec3 _velocity;
        const float _radius = 0.2; // centimeter
        const float _mass = 2.5; // gram

	public:
		ball() = default;

		glm::mat4 get_model() const;

		const glm::vec3& get_position() const;
		void set_position(const glm::vec3& value);

		const glm::vec3& get_velocity() const;
		void set_velocity(const glm::vec3& value);

        const float& get_radius() const;
        const float& get_mass() const;

		/// <summary>
		/// Get serialization result size in bytes.
		/// </summary>
		/// <returns>Number of bytes required to serialize the object.</returns>
		size_t get_serialize_size();
		/// <summary>
		/// Serialize the object to a buffer.
		/// </summary>
		/// <param name="buffer">The buffer to serialize the object to.</param>
		void serialize(unsigned char* buffer);
	};
}

#endif // !LIBTENNIS_BALL
