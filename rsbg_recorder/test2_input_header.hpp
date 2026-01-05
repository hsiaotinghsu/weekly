// This file is auto-generated.

#ifndef GENERATED_SCHEMAS_H_
#define GENERATED_SCHEMAS_H_

#include <rocksolid/dsdk.h>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <string>
#include <vector>
#include <mutex>

namespace dsdk {


namespace generated {

//-------------------------------------------------------
//
//  Schema: "Forward Camera" --> forward_camera_schema
//
//-------------------------------------------------------

struct forward_camera_schema final
{
  explicit forward_camera_schema(const std::shared_ptr<dsdk::schemas> schemas)
  {
    schema = schemas->get_schema_by_name("Forward Camera");
    if (schema == nullptr) {
      fprintf(stderr, "Schema 'Forward Camera' not found\n");
      assert(schema != nullptr);
    }

    // Array field
    camera_image = schema->get_field_id("Camera image");
    {
      const auto inf = schema->get_array_info(camera_image);
      if (inf.element_type == dsdk::type::NONE) {
        fprintf(stderr, "Array error: schema 'Forward Camera' field 'Camera image'\n");
        return;
      }
      camera_image_data.resize(static_cast<std::size_t>(inf.width * inf.rows));
      if (bind_camera_image() != dsdk::result::kOk) {
        fprintf(stderr, "Bind error: schema 'Forward Camera' field 'Camera image'\n");
        return;
      }
    }
  }

  dsdk::schema_ptr schema{};

  [[nodiscard]] std::scoped_lock<std::mutex> guard_schema()
  {
    return std::scoped_lock<std::mutex>{schema_mutex};
  }

private:
  std::mutex schema_mutex;

  //-------------------------------------------------------
  //
  //  Field "Camera image" --> camera_image
  //
  //-------------------------------------------------------

public:
  dsdk::result bind_camera_image()
  {
    return schema->bind_array<dsdk::rgb8>(camera_image, camera_image_data.data(), camera_image_data.size());
  }

  array<dsdk::rgb8> guard_camera_image()
  {
    return array<dsdk::rgb8>(camera_image_data, schema->guard_array(camera_image));
  }

private:
  dsdk::schema::field_id camera_image{};
  std::vector<dsdk::rgb8> camera_image_data;

};

//-------------------------------------------------------
//
//  Schemas container class
//
//-------------------------------------------------------

class schemas final
{
public:
  explicit schemas(dsdk::schemas_ptr handle) :
    handle(handle)
   ,forward_camera(handle)
  {
  }

private:
  dsdk::schemas_ptr handle;

public:
  forward_camera_schema forward_camera;
};

} // namespace generated

} // namespace dsdk

#endif // GENERATED_SCHEMAS_H_
