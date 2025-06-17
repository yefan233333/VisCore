#include "vis_core/feature/standard_rect/standard_rect.h"
#include "vis_core/feature/standard_rect/standard_rect_detector.h"

using namespace std;
using namespace cv;

auto StandardRect::getDetector() -> shared_ptr<StandardRectDetector>
{
    auto detector = StandardRectDetector::create();
    return detector;
}


