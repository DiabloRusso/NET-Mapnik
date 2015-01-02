#pragma once

#include <mapnik\proj_transform.hpp>
#include <mapnik\geometry.hpp>
#include <mapnik\geometry_container.hpp>
#include <mapnik\util\path_iterator.hpp>
#include <boost\spirit\include\support_container.hpp>

namespace mapnik {

	template <typename Geometry>
	struct proj_transform_adapter : noncopyable
	{
		using size_type = std::size_t;
		using value_type = typename Geometry::value_type;

		proj_transform_adapter(Geometry & geom,
			proj_transform const& prj_trans)
			: geom_(geom),
			prj_trans_(prj_trans) {}

		unsigned vertex(double *x, double *y) const
		{
			unsigned command;
			bool ok = false;
			bool skipped_points = false;
			while (!ok)
			{
				command = geom_.vertex(x, y);
				if (command == SEG_END)
				{
					return command;
				}
				double z = 0;
				ok = prj_trans_.forward(*x, *y, z);
				if (!ok) {
					skipped_points = true;
				}
			}
			if (skipped_points && (command == SEG_LINETO))
			{
				command = SEG_MOVETO;
			}
			return command;
		}

		void rewind(unsigned pos) const
		{
			geom_.rewind(pos);
		}

		unsigned type() const
		{
			return static_cast<unsigned>(geom_.type());
		}

		size_type size() const
		{
			return geom_.size();
		}

	private:
		Geometry & geom_;
		proj_transform const& prj_trans_;
	};

}

namespace NETMapnik {
	using proj_transform_path_type = mapnik::proj_transform_adapter<mapnik::geometry_type>;
	using proj_transform_container = boost::ptr_vector<proj_transform_path_type>;
}

namespace boost {
	namespace spirit {
		namespace traits {

			template <>
			struct is_container<NETMapnik::proj_transform_path_type const> : mpl::true_{};

			template <>
			struct is_container<NETMapnik::proj_transform_container const> : mpl::false_{};

			template <>
			struct container_iterator<NETMapnik::proj_transform_path_type const>
			{
				using type = mapnik::util::path_iterator<NETMapnik::proj_transform_path_type>;
			};

			template <>
			struct begin_container<NETMapnik::proj_transform_path_type const>
			{
				static mapnik::util::path_iterator<NETMapnik::proj_transform_path_type>
					call(NETMapnik::proj_transform_path_type const& g)
				{
					return mapnik::util::path_iterator<NETMapnik::proj_transform_path_type>(g);
				}
			};

			template <>
			struct end_container<NETMapnik::proj_transform_path_type const>
			{
				static mapnik::util::path_iterator<NETMapnik::proj_transform_path_type>
					call(NETMapnik::proj_transform_path_type const&)
				{
					return mapnik::util::path_iterator<NETMapnik::proj_transform_path_type>();
				}
			};

		}
	}
}