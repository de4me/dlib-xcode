// Copyright (C) 2015  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_GPU_DaTA_H_
#define DLIB_GPU_DaTA_H_

#include <dlib/gpu_data_abstract.h>
#include <memory>
#include <cstring>
#include <dlib/cuda_errors.h>
#include <dlib/serialize.h>

namespace dlib
{

// ----------------------------------------------------------------------------------------

    class gpu_data 
    {
        /*!
            CONVENTION
                - if (size() != 0) then
                    - data_host == a pointer to size() floats in CPU memory.
                - if (data_device) then 
                    - data_device == a pointer to size() floats in device memory.

                - if (there might be an active async transfer from host to device) then
                    - have_active_transfer == true

                - We use the host_current and device_current bools to keep track of which
                  copy of the data (or both) are most current.  e.g. if the CPU has
                  modified the data and it hasn't been copied to the device yet then
                  host_current==true and device_current==false.

                  Similarly, we use device_in_use==true to indicate that device() has been
                  called and no operation to wait for all CUDA kernel completion has been
                  executed.  So if device_in_use==true then there might be a CUDA kernel
                  executing that is using the device memory block contained in this object.

        !*/
    public:

        gpu_data(
        ) : data_size(0), host_current(true), device_current(true),have_active_transfer(false),device_in_use(false), the_device_id(0)
        {
        }

        // Not copyable
        gpu_data(const gpu_data&) = delete;
        gpu_data& operator=(const gpu_data&) = delete;

        // but is movable
        gpu_data(gpu_data&& item) : gpu_data() { swap(item); }
        gpu_data& operator=(gpu_data&& item) { swap(item); return *this; }

        int device_id() const { return the_device_id; }

#ifdef DLIB_USE_CUDA
        void async_copy_to_device() const; 
        void set_size(size_t new_size);
#else
        // Note that calls to host() or device() will block until any async transfers are complete.
        void async_copy_to_device() const{}

        void set_size(size_t new_size)
        {
            if (new_size == 0)
            {
                data_size = 0;
                host_current = true;
                device_current = true;
                device_in_use = false;
                data_host.reset();
                data_device.reset();
            }
            else if (new_size != data_size)
            {
                data_size = new_size;
                host_current = true;
                device_current = true;
                device_in_use = false;
                data_host.reset(new float[new_size], std::default_delete<float[]>());
                data_device.reset();
            }
        }
#endif

        const float* host() const 
        { 
            copy_to_host();
            return data_host.get(); 
        }

        float* host() 
        {
            copy_to_host();
            device_current = false;
            return data_host.get(); 
        }

        float* host_write_only() 
        {
            host_current = true;
            device_current = false;
            return data_host.get(); 
        }

        const float* device() const 
        { 
#ifndef DLIB_USE_CUDA
            DLIB_CASSERT(false, "CUDA NOT ENABLED");
#endif
            copy_to_device();
            device_in_use = true;
            return data_device.get(); 
        }

        float* device() 
        {
#ifndef DLIB_USE_CUDA
            DLIB_CASSERT(false, "CUDA NOT ENABLED");
#endif
            copy_to_device();
            host_current = false;
            device_in_use = true;
            return data_device.get(); 
        }

        float* device_write_only()
        {
#ifndef DLIB_USE_CUDA
            DLIB_CASSERT(false, "CUDA NOT ENABLED");
#endif
            wait_for_transfer_to_finish();
            host_current = false;
            device_current = true;
            device_in_use = true;
            return data_device.get(); 
        }

        bool host_ready (
        ) const { return host_current; }

        bool device_ready (
        ) const { return device_current && !have_active_transfer; }

        size_t size() const { return data_size; }

        void swap (gpu_data& item)
        {
            std::swap(data_size, item.data_size);
            std::swap(host_current, item.host_current);
            std::swap(device_current, item.device_current);
            std::swap(have_active_transfer, item.have_active_transfer);
            std::swap(data_host, item.data_host);
            std::swap(data_device, item.data_device);
            std::swap(cuda_stream, item.cuda_stream);
            std::swap(the_device_id, item.the_device_id);
        }

    private:

#ifdef DLIB_USE_CUDA
        void copy_to_device() const;
        void copy_to_host() const;
        void wait_for_transfer_to_finish() const;
#else
        void copy_to_device() const{}
        void copy_to_host() const{}
        void wait_for_transfer_to_finish() const{}
#endif


        size_t data_size;
        mutable bool host_current;
        mutable bool device_current;
        mutable bool have_active_transfer;
        mutable bool device_in_use;

        std::shared_ptr<float> data_host;
        std::shared_ptr<float> data_device;
        std::shared_ptr<void> cuda_stream;
        int the_device_id;
    };

    inline void serialize(const gpu_data& item, std::ostream& out)
    {
        int version = 1;
        serialize(version, out);
        serialize(item.size(), out);
        auto data = item.host();
        for (size_t i = 0; i < item.size(); ++i)
            serialize(data[i], out);
    }

    inline void deserialize(gpu_data& item, std::istream& in)
    {
        int version;
        deserialize(version, in);
        if (version != 1)
            throw serialization_error("Unexpected version found while deserializing dlib::gpu_data.");
        size_t s;
        deserialize(s, in);
        item.set_size(s);
        auto data = item.host();
        for (size_t i = 0; i < item.size(); ++i)
            deserialize(data[i], in);
    }

#ifdef DLIB_USE_CUDA
    void memcpy (gpu_data& dest, const gpu_data& src);

    void memcpy (
        gpu_data& dest, 
        size_t dest_offset,
        const gpu_data& src,
        size_t src_offset,
        size_t num
    );

#else

    inline void memcpy (gpu_data& dest, const gpu_data& src)
    {
        DLIB_CASSERT(dest.size() == src.size());
        if (src.size() == 0 || &dest == &src)
            return;
        std::memcpy(dest.host_write_only(), src.host(), sizeof(float)*src.size());
    }

    inline void memcpy (
        gpu_data& dest, 
        size_t dest_offset,
        const gpu_data& src,
        size_t src_offset,
        size_t num
    )
    {
        DLIB_CASSERT(dest_offset + num <= dest.size());
        DLIB_CASSERT(src_offset + num <= src.size());
        if (num == 0)
            return;
        if (&dest == &src && std::max(dest_offset, src_offset) < std::min(dest_offset,src_offset)+num)
        {
            // if they perfectly alias each other then there is nothing to do
            if (dest_offset == src_offset)
                return;
            else
                std::memmove(dest.host()+dest_offset, src.host()+src_offset, sizeof(float)*num);
        }
        else
        {
            // if we write to the entire thing then we can use host_write_only()
            if (dest_offset == 0 && num == dest.size())
                std::memcpy(dest.host_write_only(), src.host()+src_offset, sizeof(float)*num);
            else
                std::memcpy(dest.host()+dest_offset, src.host()+src_offset, sizeof(float)*num);
        }
    }
#endif

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_GPU_DaTA_H_

