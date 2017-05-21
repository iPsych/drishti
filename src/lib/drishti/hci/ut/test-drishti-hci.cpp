/*!
  @file   test-hci.cpp
  @author David Hirvonen
  @brief  Google test for public drishti API.

  \copyright Copyright 2016 Elucideye, Inc. All rights reserved.
  \license{This project is released under the 3 Clause BSD License.}

  Note that drishti::hci:FaceFinder has a dependency on an active OpenGL rendering context,
  which is used (at the least) for creating upright rescaled video frames for ACF channel 
  computation, or (at the most) full ACF channel computation on the GPU using shaders.

*/

// clang-format off
#if defined(DRISHTI_HCI_DO_GPU) && defined(DRISHTI_BUILD_QT)
#  include <QApplication>
#endif
// clang-format on

#include <gtest/gtest.h>

#include <fstream>

extern const char* sFaceDetector;
extern const char* sFaceDetectorMean;
extern const char* sFaceRegressor;
extern const char* sEyeRegressor;
extern const char* sImageFilename;

static bool hasFile(const std::string& filename)
{
    std::ifstream ifs(filename);
    return ifs.good();
}

int drishti_main(int argc, char** argv)
{
#if defined(DRISHTI_BUILD_QT) && defined(DRISHTI_HCI_DO_GPU)
    QApplication app(argc, argv);
#endif

    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 6);

    sFaceDetector = argv[1];
    sFaceDetectorMean = argv[2];
    sFaceRegressor = argv[3];
    sEyeRegressor = argv[4];
    sImageFilename = argv[5];

    assert(hasFile(sFaceDetector));
    assert(hasFile(sFaceDetectorMean));
    assert(hasFile(sFaceRegressor));
    assert(hasFile(sEyeRegressor));
    assert(hasFile(sImageFilename));

    return RUN_ALL_TESTS();
}
