#include <opencv2/opencv.hpp>


#include "vis_core/geom_utils/geometry.h"
#include "vis_core/contour_proc/contour_wrapper.hpp"
#include "vis_core/img_proc/image_wrapper.hpp"
#include "vis_core/feature_node/feature_node.h"
#include "vis_core/pose_proc/transform6D.hpp"

// 定义绘制颜色常量
const cv::Scalar RED(0, 0, 255);
const cv::Scalar GREEN(0, 255, 0);
const cv::Scalar BLUE(255, 0, 0);
const cv::Scalar YELLOW(0, 255, 255);
const cv::Scalar CYAN(255, 255, 0);
const cv::Scalar MAGENTA(255, 0, 255);
const cv::Scalar WHITE(255, 255, 255);
const cv::Scalar BLACK(0, 0, 0);

// 转换坐标以适配OpenCV坐标系（y轴向下）
cv::Point2f cvPoint(const cv::Point2f &p)
{
    return cv::Point2f(p.x, p.y);
}

/**
 * 测试直线类功能
 */
void testLine(cv::Mat &canvas)
{

    // 创建直线
    Line2f l1(0, cv::Point2f(100, 100));                     // 0°水平线
    Line2f l2(CV_PI / 2, cv::Point2f(200, 200));             // 90°垂直线
    Line2f l3(cv::Point2f(300, 100), cv::Point2f(400, 200)); // 对角线

    // 计算交点
    auto intersections = l1.intersect(l2);
    auto intersections2 = l1.intersect(l3);

    // 绘制直线
    cv::Point2f d1 = l1.direction() * 200;
    cv::line(canvas, cvPoint(l1.point()), cvPoint(l1.point() + d1), RED, 2);
    cv::line(canvas, cvPoint(l1.point()), cvPoint(l1.point() - d1), RED, 2);

    cv::Point2f d2 = l2.direction() * 200;
    cv::line(canvas, cvPoint(l2.point()), cvPoint(l2.point() + d2), GREEN, 2);
    cv::line(canvas, cvPoint(l2.point()), cvPoint(l2.point() - d2), GREEN, 2);

    cv::Point2f d3 = l3.direction() * 200;
    cv::line(canvas, cvPoint(l3.point()), cvPoint(l3.point() + d3), BLUE, 2);
    cv::line(canvas, cvPoint(l3.point()), cvPoint(l3.point() - d3), BLUE, 2);

    // 标记交点
    for (auto &p : intersections)
    {
        cv::circle(canvas, cvPoint(p), 8, YELLOW, -1);
    }

    for (auto &p : intersections2)
    {
        cv::circle(canvas, cvPoint(p), 8, CYAN, -1);
    }

    // 测试点投影
    cv::Point2f testPoint(350, 150);
    cv::circle(canvas, cvPoint(testPoint), 8, MAGENTA, -1);

    cv::Point2f proj = l3.project(testPoint);
    cv::circle(canvas, cvPoint(proj), 6, WHITE, -1);
    cv::line(canvas, cvPoint(testPoint), cvPoint(proj), WHITE, 1, cv::LINE_AA);

    // 显示角度信息
    std::string text = "Line Tests:";
    putText(canvas, text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 2);
    text = "Red: 0° Line, Green: 90° Line, Blue: Diagonal";
    putText(canvas, text, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 1);
    text = "Yellow: Intersection, Cyan: Projection";
    putText(canvas, text, cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 1);
}

/**
 * 测试圆类功能
 */
void testCircle(cv::Mat &canvas)
{
    // 创建圆
    Circle2f c1(cv::Point2f(150, 150), 50);
    Circle2f c2(cv::Point2f(300, 150), 100);
    Circle2f c3(cv::Point2f(200, 300), 70);

    // 创建测试直线
    Line2f l1(cv::Point2f(50, 250), cv::Point2f(350, 250)); // 水平线
    Line2f l2(CV_PI / 4, cv::Point2f(200, 100));            // 45°线

    // 绘制圆
    cv::circle(canvas, cvPoint(c1.center()), c1.radius(), RED, 2);
    cv::circle(canvas, cvPoint(c2.center()), c2.radius(), GREEN, 2);
    cv::circle(canvas, cvPoint(c3.center()), c3.radius(), BLUE, 2);

    // 绘制圆心
    cv::circle(canvas, cvPoint(c1.center()), 5, RED, -1);
    cv::circle(canvas, cvPoint(c2.center()), 5, GREEN, -1);
    cv::circle(canvas, cvPoint(c3.center()), 5, BLUE, -1);

    // 计算并绘制圆-线交点
    auto cl1 = c1.intersect(l1);
    for (auto &p : cl1)
    {
        cv::circle(canvas, cvPoint(p), 8, YELLOW, -1);
    }

    auto cl2 = c3.intersect(l2);
    for (auto &p : cl2)
    {
        cv::circle(canvas, cvPoint(p), 8, CYAN, -1);
    }

    // 计算并绘制圆圆交点
    auto cc1 = c1.intersect(c2);
    for (auto &p : cc1)
    {
        cv::circle(canvas, cvPoint(p), 8, MAGENTA, -1);
    }

    // 绘制直线
    cv::Point2f d = l1.direction() * 300;
    cv::line(canvas, cvPoint(l1.point() - d), cvPoint(l1.point() + d), WHITE, 1, cv::LINE_AA);

    cv::Point2f d2 = l2.direction() * 300;
    cv::line(canvas, cvPoint(l2.point() - d2), cvPoint(l2.point() + d2), WHITE, 1, cv::LINE_AA);

    // 显示信息
    std::string text = "Circle Tests:";
    putText(canvas, text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 2);
    text = "Red/Green/Blue: Circles, White: Lines";
    putText(canvas, text, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 1);
    text = "Yellow/Cyan: Circle-Line Intersections, Magenta: Circle-Circle";
    putText(canvas, text, cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 1);
}

/**
 * 测试线段类功能
 */
void testSegment(cv::Mat &canvas)
{
    // 创建线段
    Segment2f s1(cv::Point2f(100, 100), cv::Point2f(300, 150));
    Segment2f s2(cv::Point2f(100, 200), cv::Point2f(250, 100));
    Segment2f s3(cv::Point2f(350, 50), cv::Point2f(200, 300));

    // 测试点
    cv::Point2f testPoint(200, 150);

    // 绘制线段
    cv::line(canvas, cvPoint(s1.start()), cvPoint(s1.end()), RED, 2);
    cv::line(canvas, cvPoint(s2.start()), cvPoint(s2.end()), GREEN, 2);
    cv::line(canvas, cvPoint(s3.start()), cvPoint(s3.end()), BLUE, 2);

    // 绘制测试点
    cv::circle(canvas, cvPoint(testPoint), 8, MAGENTA, -1);

    // 计算并绘制投影和最近点
    cv::Point2f proj = s1.project(testPoint);
    cv::circle(canvas, cvPoint(proj), 6, YELLOW, -1);

    cv::Point2f closest = s1.closestPoint(testPoint);
    cv::circle(canvas, cvPoint(closest), 6, CYAN, -1);

    // 绘制投影线
    cv::line(canvas, cvPoint(testPoint), cvPoint(proj), WHITE, 1, cv::LINE_AA);
    cv::line(canvas, cvPoint(testPoint), cvPoint(closest), WHITE, 1, cv::LINE_AA);

    // 计算并绘制线段交点
    auto inter1 = s1.intersect(s2);
    for (auto &p : inter1)
    {
        cv::circle(canvas, cvPoint(p), 8, WHITE, -1);
    }

    // 显示信息
    std::string text = "Segment Tests:";
    putText(canvas, text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 2);
    text = "Red/Green/Blue: Segments, Magenta: Test Point";
    putText(canvas, text, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 1);
    text = "Yellow: Projection, Cyan: Closest Point, White: Intersection";
    putText(canvas, text, cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 1);
}

/**
 * 综合测试 - 创建三个几何对象并显示它们的关系
 */
void testCombined(cv::Mat &canvas)
{
    // 创建几何对象
    Circle2f circle(cv::Point2f(300, 200), 80);
    Segment2f segment(cv::Point2f(100, 250), cv::Point2f(450, 250));
    Line2f line(CV_PI / 4, cv::Point2f(250, 350));

    // 绘制圆
    cv::circle(canvas, cvPoint(circle.center()), circle.radius(), GREEN, 2);
    cv::circle(canvas, cvPoint(circle.center()), 5, GREEN, -1);

    // 绘制线段
    cv::line(canvas, cvPoint(segment.start()), cvPoint(segment.end()), BLUE, 2);

    // 绘制直线
    cv::Point2f d = line.direction() * 300;
    cv::line(canvas, cvPoint(line.point()), cvPoint(line.point() + d), RED, 2);
    cv::line(canvas, cvPoint(line.point()), cvPoint(line.point() - d), RED, 2);

    // 计算并显示关系
    // 直线-圆交点
    auto lcInter = circle.intersect(line);
    for (auto &p : lcInter)
    {
        cv::circle(canvas, cvPoint(p), 8, YELLOW, -1);
    }

    // 线段-圆交点
    auto scInter = circle.intersect(Line2f(segment.start(), segment.end()));
    for (auto &p : scInter)
    {
        // 检查点是否在线段上
        if (segment.contains(p))
        {
            cv::circle(canvas, cvPoint(p), 8, CYAN, -1);
        }
    }

    // 线段-直线交点
    auto slInter = segment.intersect(Segment2f(line.point(), line.point() + d));
    for (auto &p : slInter)
    {
        cv::circle(canvas, cvPoint(p), 8, MAGENTA, -1);
    }

    // 显示信息
    std::string text = "Combined Tests:";
    putText(canvas, text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 2);
    text = "Green: Circle, Blue: Segment, Red: Line";
    putText(canvas, text, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 1);
    text = "Yellow: Circle-Line, Cyan: Circle-Segment, Magenta: Segment-Line";
    putText(canvas, text, cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 0.7, WHITE, 1);
}

/**
 * @brief 去除常量和引用限定的旋转矩阵类型概念
 */
template <typename T>
using base_type = std::remove_cvref_t<T>;

int main()
{

    FeatureNode f1;
    
    Transform6D t1;
    cv::Matx33d rmat = cv::Matx33d::eye();
    cv::Matx31d tvec(0, 0, 0);

    Transform6D t2(rmat, tvec);

    // 创建窗口
    const char *winName = "Geometry Tests";
    cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);

    // 定义测试场景
    std::vector<std::pair<std::string, std::function<void(cv::Mat &)>>> tests = {
        {"Line Tests", testLine},
        {"Circle Tests", testCircle},
        {"Segment Tests", testSegment},
        {"Combined Tests", testCombined}};

    for (const auto &test : tests)
    {
        // 创建空白画布
        cv::Mat canvas(600, 600, CV_8UC3, cv::Scalar(40, 40, 40));

        // 绘制测试标题
        cv::putText(canvas, test.first, cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(100, 255, 255), 2);

        // 运行测试
        test.second(canvas);

        // 显示结果
        cv::imshow(winName, canvas);

        // 等待按键继续
        int key = cv::waitKey(0);
        if (key == 27)
            break; // ESC键退出
    }

    cv::destroyAllWindows();
    return 0;
}