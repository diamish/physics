#include "HDF5Reader.h"
#include <H5Cpp.h>
#include <stdexcept>
#include <iostream>

using namespace H5;

StrainSeries HDF5Reader::read_strain(const std::string& file_path, const std::string& dataset) {
    StrainSeries out;
    try {
        H5File file(file_path, H5F_ACC_RDONLY);

        // Attempt to open dataset path commonly used in GWOSC HDF5 files.
        DataSet ds = file.openDataSet(dataset);
        DataSpace dsp = ds.getSpace();
        hsize_t dims_out[1];
        int ndims = dsp.getSimpleExtentDims(dims_out, NULL);
        size_t n = dims_out[0];

        // Read dataset to vector
        out.data.resize(n);
        ds.read(out.data.data(), PredType::NATIVE_DOUBLE);

        // Try to read attributes: 'Xstart' or 'StartTime' or 'GPSstart' (common attr names vary)
        if (ds.attrExists("Xstart")) {
            Attribute a = ds.openAttribute("Xstart");
            double v;
            a.read(PredType::NATIVE_DOUBLE, &v);
            out.start_time = v;
        } else if (file.attrExists("StartTime")) {
            Attribute a = file.openAttribute("StartTime");
            double v; a.read(PredType::NATIVE_DOUBLE, &v); out.start_time = v;
        } else {
            // Fallback: 0
            out.start_time = 0.0;
        }

        // sampling rate: try attribute 'Xspacing' or 'SampleRate'
        if (ds.attrExists("Xspacing")) {
            Attribute a = ds.openAttribute("Xspacing");
            double v; a.read(PredType::NATIVE_DOUBLE, &v);
            if (v > 0) out.sample_rate = 1.0 / v;
        } else if (file.attrExists("SampleRate")) {
            Attribute a = file.openAttribute("SampleRate");
            double v; a.read(PredType::NATIVE_DOUBLE, &v);
            if (v > 0) out.sample_rate = v;
        }
    } catch (FileIException &e) {
        throw std::runtime_error("HDF5 file open/read error");
    } catch (DataSetIException &e) {
        throw std::runtime_error("HDF5 dataset error");
    }
    return out;
}

