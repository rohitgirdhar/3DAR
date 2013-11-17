#ifndef CAMERA_TRANSFORMER
#define CAMERA_TRANSFORMER

#include <glm/glm.hpp>
#include <string>
#include <opencv2/opencv.hpp>

class CameraTransformer {
    cv::Mat computeHomography(cv::Mat, cv::Mat);
    void visualizeMatching(
            cv::Mat, 
            cv::Mat,
            std::vector<cv::Point2f>, 
            std::vector<cv::Point2f>);
    void refineOnF(
            std::vector<cv::Point3f>& pts3d,
            std::vector<cv::Point2f>& pts2d_output,
            std::vector<cv::Point2f>& pts2d,
            int,int);
    public:
    cv::Mat H;
    cv::Mat F;
    std::vector<cv::KeyPoint> seedPts, qPts;
    cv::Mat intCameraMat; // intrinsic camera matrix for the query image
    /**
     * function to re-caliberate camera using matching image and
     * its point correspondences
     * @param source_fname : fname of source/query image
     * @param match_fname : fname of mathching image, using BOW
     * @param center : output camera center position vector
     * @param look_dir : output look direction vector for openGL
     * @param up_dir : output up direction vectoe for openGL
     */
    void recalibrateCamera(
            std::string source_fname,
            std::string match_fname,
            glm::vec3& center,
            glm::vec3& look_dir,
            glm::vec3& up_dir);
};

#endif
