#pragma once
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300
//#define CL_HPP_MINIMUM_OPENCL_VERSION 120
//#define CL_HPP_CL_1_2_DEFAULT_BUILD
#include <CL/opencl.hpp>
#include <CL/cl_gl.h>
#include "CLTypes.h"
#include "ReadWrite.h"
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <iostream>

#ifdef linux
    #include <GL/glx.h>
#endif

inline cl_int CLBLog(cl::Program& prog)
{
    cl_int result = CL_SUCCESS;
	if (CL_COMPLOG) {
        auto buildInfo = prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(&result);
		std::ofstream clfile(GLOBALS::DATA_FOLDER+CL_BUILD_LOG);
		if (clfile.is_open()) {
            for (auto &pair : buildInfo) {
                clfile << pair.second << std::endl << std::endl;
            }
			clfile.close();
		}
	}
	return result;
}

class CL
{
private:
	cl::Platform platform;
	cl::Device device;
	cl::Device host_dev;
	cl::Program program;
    cl::Kernel GH_Kernel;
    cl::Kernel GN_Kernel;
    cl::Kernel GS_Kernel;
    cl::Kernel GP_Kernel;
    cl::Kernel DP_Kernel;
    cl::Kernel DM_Kernel;
    cl::Kernel DS_Kernel;
    cl::Kernel DG_Kernel;
    cl::Kernel PS_Kernel;
    cl::Kernel PP_Kernel;
    cl::Kernel FB_Kernel;
    cl::Kernel FF_Kernel;

public:
	//cl::DeviceCommandQueue dev_queue;
	cl::CommandQueue queue;
	cl::Context gpu_context;
	uint32_t max_wg_size;

	void Initialize() {
		std::cout << "Initializing OpenCL ... ";

		// get all platforms (drivers)
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
        cl::Platform plat;

		if (platforms.size() == 0) {
			HandleFatalError(27, "No OpenCL platforms found");
		}

        for (auto &p : platforms) {
            std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
            if (platver.find("OpenCL 3.") != std::string::npos) {
                plat = p;
            }
        }

        if (plat() == 0) {
            HandleFatalError(28, "No OpenCL 3 platform found.");
        }

        platform = cl::Platform::setDefault(plat);
        if (platform != plat) {
            HandleFatalError(29, "Error setting default platform.");
        }

		// get host system compute devices
		cl::Context cpu_context = cl::Context(CL_DEVICE_TYPE_CPU);
		std::vector<cl::Device> cpu_devices = cpu_context.getInfo<CL_CONTEXT_DEVICES>();

		if (cpu_devices.size() == 0) {
			HandleFatalError(30, "No supported host compute devices found");
		} else {
            host_dev = cpu_devices[0];
		}

        #ifdef linux
            cl_context_properties props[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
            CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0};
        #elif defined WIN32 || defined _WIN32
            cl_context_properties props[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
            CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0};
        #elif defined(__APPLE__) || defined(MACOSX)
            CGLContextObj glContext = CGLGetCurrentContext();
            CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);
            cl_context_properties props[] = {
            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
            (cl_context_properties)shareGroup, 0};
        #endif

		// get gpu compute devices
        gpu_context = cl::Context(CL_DEVICES_FOR_GL_CONTEXT_KHR, props);
		std::vector<cl::Device> gpu_devices = gpu_context.getInfo<CL_CONTEXT_DEVICES>();

		if (gpu_devices.size() == 0)
			HandleFatalError(31, "No supported opengl compute devices found");

		// select a compute device
        std::string gpu_index = GLOBALS::config_map["GPU_INDEX"];
        if (gpu_index == "primary") {
            device = cl::Device::setDefault(gpu_devices[0]);
        } else {
            size_t gi = stoi(gpu_index);
            if (gi < gpu_devices.size()) {
                device = cl::Device::setDefault(gpu_devices[gi]);
            } else {
                HandleFatalError(33, "Invalid GPU_INDEX, check settings");
            }
        }

		// verifying CL/GL sharing is supported on device
		std::string dev_exts = device.getInfo<CL_DEVICE_EXTENSIONS>();
		if (dev_exts.find(CL_GL_SHARING_EXT) == std::string::npos) {
			HandleFatalError(34, "Device does not support CL/GL sharing");
		} else if (device.getInfo<CL_DEVICE_IMAGE_SUPPORT>()!=CL_TRUE) {
			HandleFatalError(35, "Device does not support OpenCL images");
		} else {
            std::cout << "Success!" << std::endl;
        }

		// Read kernel source files
		std::string sourceCode(ReadFileStr(GLOBALS::DATA_FOLDER+"kernels/variables.cl"));
		sourceCode += ReadFileStr(GLOBALS::DATA_FOLDER+"kernels/functions.cl");
		sourceCode += ReadFileStr(GLOBALS::DATA_FOLDER+"kernels/compute.cl");

		std::vector<std::string> sources;
		sources.push_back(sourceCode);

		// Set program source code and context
		program = cl::Program(gpu_context, sources);

		// build kernel program and check for errors
		std::cout << "Building OpenCL kernels ... ";
		try {
            if (program.build(device, "-cl-std=CL3.0") != CL_SUCCESS) {
                std::cout << "Failed!" << std::endl;
                CLBLog(program);
                HandleFatalError(36, "Failed building kernel program");
            } else {
                std::cout << "Success!" << std::endl;
                CLBLog(program);
            }
		} catch (...) {
            CLBLog(program);
            HandleFatalError(37, "Failed building kernel program");
		}

        try {
            queue = cl::CommandQueue(gpu_context, device);
            //dev_queue = cl::DeviceCommandQueue::makeDefault(gpu_context, device);
        } catch (cl::Error& e) {
            HandleFatalError(38, "Failed to create command queue");
            exit(EXIT_FAILURE);
        }

        // initialize kernel objects
		GH_Kernel = cl::Kernel(program, "GenHeightMap");
		GN_Kernel = cl::Kernel(program, "GenNormalMap");
		GS_Kernel = cl::Kernel(program, "GenStars");
		GP_Kernel = cl::Kernel(program, "GenPlanets");
		DP_Kernel = cl::Kernel(program, "DrawPlanet");
		DM_Kernel = cl::Kernel(program, "DrawMoon");
		DS_Kernel = cl::Kernel(program, "DrawStar");
		DG_Kernel = cl::Kernel(program, "DrawStars");
		PS_Kernel = cl::Kernel(program, "MousePickStar");
		PP_Kernel = cl::Kernel(program, "MousePickPlanet");
		FB_Kernel = cl::Kernel(program, "FillFragBuff");
		FF_Kernel = cl::Kernel(program, "FragsToFrame");

		max_wg_size = (cl_uint)device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

        PrintCLInfo();
	}

	void PrintCLInfo()
	{
		std::cout << "\nUsing platform: "+platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
		std::cout << "Using device: "+device.getInfo<CL_DEVICE_NAME>()+" ("+device.getInfo<CL_DEVICE_VENDOR>()+")" << std::endl;
		std::cout << "OpenCL version: "+device.getInfo<CL_DEVICE_VERSION>() << std::endl;
		std::cout << "Max parameter size: "+VarToStr((cl_uint)device.getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>())+" KB" << std::endl;
		std::cout << "Constant buffer size: "+VarToStr((cl_ulong)device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>()/1024)+" KB" << std::endl;
		std::cout << "Local memory size: "+VarToStr((cl_ulong)device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>()/1024)+" KB" << std::endl;
		std::cout << "Global memory size: "+VarToStr((cl_ulong)device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()/1024/1024)+" MB" << std::endl;
		std::cout << "Max compute units: "+VarToStr((cl_uint)device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>()) << std::endl;
		std::cout << "Max workgroup size: "+VarToStr(max_wg_size) << std::endl << std::endl;
	}

	void WriteToBuffer(cl::Buffer& buff, const void* data_ptr, const cl_uint data_size, const cl_uint offset=0)
	{
        queue.enqueueWriteBuffer(buff, CL_TRUE, offset, data_size, data_ptr);
        queue.finish();
	}

	void GenHeightMap(const bool is_planet, const uint64_t seed, cl_float water_frac, cl_uint width, cl_uint height, cl::Buffer& terrain_map)
    {
        uint64_t seed1 = RandomLong(seed);
        uint64_t seed2 = RandomLong(seed1);
        float randFlt = FltFromLong(seed1) * 10000.0f;
        float mountMix = ((FltFromLong(seed2) * 0.9f) + (1.0f-water_frac)) * 0.5f;

        #if CL_DEBUG
        try {
        #endif
        GH_Kernel.setArg(0, terrain_map);
        GH_Kernel.setArg(1, randFlt);
        GH_Kernel.setArg(2, mountMix);
        GH_Kernel.setArg(3, (cl_uchar)is_planet);
        queue.enqueueNDRangeKernel(GH_Kernel, cl::NullRange, cl::NDRange(width, height));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::GenHeightMap() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
    }

	void GenNormalMap(cl_uint width, cl_uint height, cl::Buffer& terrain_map, cl::Buffer& out_normals)
    {
        #if CL_DEBUG
        try {
        #endif
        GN_Kernel.setArg(0, terrain_map);
        GN_Kernel.setArg(1, out_normals);
        queue.enqueueNDRangeKernel(GN_Kernel, cl::NullRange, cl::NDRange(width, height));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::GenNormalMap() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
    }

	/*void GenWaterMap(cl_uint width, cl_uint height, cl_float seed, cl_float scale, cl_uchar detail, cl::Buffer& water_map)
    {
        #if CL_DEBUG
        try {
        #endif
        GW_Kernel.setArg(0, water_map);
        GW_Kernel.setArg(1, seed);
        GW_Kernel.setArg(2, scale);
        GW_Kernel.setArg(3, detail);
        queue.enqueueNDRangeKernel(GW_Kernel, cl::NullRange, cl::NDRange(width, height));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::GenWaterMap() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
    }*/

	void GenStars(cl::Buffer& cl_star_buff, const cl_RenderInfo& render_info)
	{
        #if CL_DEBUG
        try {
        #endif
        GS_Kernel.setArg(0, cl_star_buff);
        GS_Kernel.setArg(1, render_info);
		queue.enqueueNDRangeKernel(GS_Kernel, cl::NullRange, cl::NDRange(SECTOR_SPAN,SECTOR_SPAN,SECTOR_SPAN));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::GenStars() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}

	void GenPlanets(cl_uchar count, cl::Buffer& cl_system_buff, cl::Buffer& cl_planet_buff,
                 cl::Buffer& cl_moon_buff, cl::Buffer& cl_star_buff, cl::Buffer& cl_index_buff)
	{
        #if CL_DEBUG
        try {
        #endif
        GP_Kernel.setArg(0, cl_system_buff);
        GP_Kernel.setArg(1, cl_planet_buff);
        GP_Kernel.setArg(2, cl_moon_buff);
        GP_Kernel.setArg(3, cl_star_buff);
        GP_Kernel.setArg(4, cl_index_buff);
		queue.enqueueNDRangeKernel(GP_Kernel, cl::NullRange, cl::NDRange(count));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::GenPlanets() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}

	void DrawMoon(uint4& ranges, cl::Buffer& cl_moon_buff, cl::Buffer& cl_star_buff, cl::Buffer& cl_frag_buff,
    std::vector<cl::Buffer>& cl_mmap_buffs, std::vector<cl::Image2D>& cl_moon_texs, cl_uint moon_index,
    cl_uint loaded_index, const cl_Planet& planet, const cl_RenderInfo& render_info)
	{
        #if CL_DEBUG
        try {
        #endif
        DM_Kernel.setArg(0, cl_moon_buff);
        DM_Kernel.setArg(1, cl_star_buff);
        DM_Kernel.setArg(2, cl_frag_buff);
        DM_Kernel.setArg(3, cl_mmap_buffs[moon_index]);
        DM_Kernel.setArg(4, cl_moon_texs[moon_index]);
        DM_Kernel.setArg(5, moon_index);
        DM_Kernel.setArg(6, loaded_index);
        DM_Kernel.setArg(7, planet);
        DM_Kernel.setArg(8, render_info);
		queue.enqueueNDRangeKernel(DM_Kernel, cl::NDRange(ranges.x, ranges.y), cl::NDRange(ranges.z, ranges.w));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::DrawMoon() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}

	void DrawPlanet(uint4& ranges, cl::Buffer& cl_planet_buff, cl::Buffer& cl_moon_buff, cl::Buffer& cl_star_buff,
    cl::Buffer& cl_frag_buff, std::vector<cl::Buffer>& cl_pmap_buffs, std::vector<cl::Image2D>& cl_planet_texs,
    cl_uint planet_index, cl_uint loaded_index, const cl_RenderInfo& render_info)
	{
        #if CL_DEBUG
        try {
        #endif
        DP_Kernel.setArg(0, cl_planet_buff);
        DP_Kernel.setArg(1, cl_moon_buff);
        DP_Kernel.setArg(2, cl_star_buff);
        DP_Kernel.setArg(3, cl_frag_buff);
        DP_Kernel.setArg(4, cl_pmap_buffs[planet_index]);
        DP_Kernel.setArg(5, cl_planet_texs[planet_index]);
        DP_Kernel.setArg(6, planet_index);
        DP_Kernel.setArg(7, loaded_index);
        DP_Kernel.setArg(8, render_info);
		queue.enqueueNDRangeKernel(DP_Kernel, cl::NDRange(ranges.x, ranges.y), cl::NDRange(ranges.z, ranges.w));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::DrawPlanet() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}

	void DrawStar(uint4& ranges, cl::Buffer& cl_star_buff, cl::Buffer& cl_frag_buff,
               cl_uint loaded_index, cl_double& g_time, const cl_RenderInfo& render_info)
	{
        #if CL_DEBUG
        try {
        #endif
        DS_Kernel.setArg(0, cl_star_buff);
        DS_Kernel.setArg(1, cl_frag_buff);
        DS_Kernel.setArg(2, loaded_index);
        DS_Kernel.setArg(3, g_time);
        DS_Kernel.setArg(4, render_info);
		queue.enqueueNDRangeKernel(DS_Kernel, cl::NDRange(ranges.x, ranges.y), cl::NDRange(ranges.z, ranges.w));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::DrawStar() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}

	void DrawStars(cl::Memory& pixel_buff, cl::Memory& idata_buffer, cl::Buffer& cl_star_buff,
                cl::Buffer& cl_idat_sizes_buff, cl::Buffer& cl_index_buff, const cl_RenderInfo& render_info)
	{
        #if CL_DEBUG
        try {
        #endif
        DG_Kernel.setArg(0, pixel_buff);
        DG_Kernel.setArg(1, idata_buffer);
        DG_Kernel.setArg(2, cl_star_buff);
        DG_Kernel.setArg(3, cl_idat_sizes_buff);
        DG_Kernel.setArg(4, cl_index_buff);
        DG_Kernel.setArg(5, render_info);
		queue.enqueueNDRangeKernel(DG_Kernel, cl::NullRange, cl::NDRange(SECTOR_SPAN,SECTOR_SPAN,SECTOR_SPAN));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::DrawStars() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}

	void PickStar(cl::Buffer& cl_star_buff, cl::Buffer& cl_volatile_buff, cl::Buffer& cl_index_buff,
                cl_float2& cl_mouse_pos, const cl_RenderInfo& render_info)
	{
        #if CL_DEBUG
        try {
        #endif
        PS_Kernel.setArg(0, cl_star_buff);
        PS_Kernel.setArg(1, cl_volatile_buff);
        PS_Kernel.setArg(2, cl_index_buff);
        PS_Kernel.setArg(3, cl_mouse_pos);
        PS_Kernel.setArg(4, render_info);
		queue.enqueueNDRangeKernel(PS_Kernel, cl::NullRange, cl::NDRange(SECTOR_SPAN,SECTOR_SPAN,SECTOR_SPAN));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::PickStar() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}

	void PickPlanet(cl_uchar count, cl::Buffer& cl_planet_buff, cl::Buffer& cl_moon_buff, cl::Buffer& cl_volatile_buff,
                    cl::Buffer& cl_index_buff, cl_float2& cl_mouse_pos, const cl_RenderInfo& render_info)
	{
        #if CL_DEBUG
        try {
        #endif
        PP_Kernel.setArg(0, cl_planet_buff);
        PP_Kernel.setArg(1, cl_moon_buff);
        PP_Kernel.setArg(2, cl_volatile_buff);
        PP_Kernel.setArg(3, cl_index_buff);
        PP_Kernel.setArg(4, cl_mouse_pos);
        PP_Kernel.setArg(5, render_info);
		queue.enqueueNDRangeKernel(PP_Kernel, cl::NullRange, cl::NDRange(count));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::PickPlanet() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}

	void FillFragBuff(cl_uint ww, cl_uint wh, cl::Buffer& cl_frag_buff, const cl_RenderInfo& render_info)
	{
        #if CL_DEBUG
        try {
        #endif
        FB_Kernel.setArg(0, cl_frag_buff);
        FB_Kernel.setArg(1, render_info);
		queue.enqueueNDRangeKernel(FB_Kernel, cl::NullRange, cl::NDRange(ww, wh));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::FillFragBuff() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}

	void FragsToFrame(cl_uint ww, cl_uint wh, cl::Buffer& cl_frag_buff, cl::Memory& pixel_buff, const cl_RenderInfo& render_info)
	{
        #if CL_DEBUG
        try {
        #endif
        FF_Kernel.setArg(0, cl_frag_buff);
        FF_Kernel.setArg(1, pixel_buff);
        FF_Kernel.setArg(2, render_info);
		queue.enqueueNDRangeKernel(FF_Kernel, cl::NullRange, cl::NDRange(ww, wh));
        #if CL_DEBUG
        } catch (cl::Error& e) {
            PrintLine("[CL::FragsToFrame() Error] ("+VarToStr(e.err())+"): "+e.what());
            exit(EXIT_FAILURE);
        }
        #endif
	}
};
