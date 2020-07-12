#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma ide diagnostic ignored "OCSimplifyInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef HC_GEOMETRY_H
#define HC_GEOMETRY_H

#include <ostream>
#include <vector>
#include <cmath>
#include <cassert>

namespace hc {

    /**
     * General point class that can have an optional identifier attached.
     *
     * THe sentinel -1 is used for all points with no identifier.
     */
    class Point {
        int id_;
        int x_;
        int y_;
    public:
        /**
         *
         * @param id Must be at most 2^31-1
         * @param x the x coordinate
         * @param y the y coordinate
         */
        Point(const unsigned int id, const int x, const int y) : id_(static_cast<int>(id)), x_(x), y_(y) {
            assert(id == id_);
        }

        /**
         * Sets the id to sentinel value -1.
         *
         * @param x the x coordinate
         * @param y the y coordinate
         */
        Point(int x, int y) : id_(-1), x_(x), y_(y) {}

        Point(const Point&) = default;
        Point(Point&&) = default;
        Point& operator=(const Point&) = default;
        Point& operator=(Point&&) = default;


        [[nodiscard]] int id() const {
            return id_;
        }

        [[nodiscard]] int x() const {
            return x_;
        }

        [[nodiscard]] int y() const {
            return y_;
        }

        bool operator==(const Point &rhs) const {
            return id_ == rhs.id_ &&
                   x_ == rhs.x_ &&
                   y_ == rhs.y_;
        }

        bool operator!=(const Point &rhs) const {
            return !(rhs == *this);
        }

        bool operator<(const Point &rhs) const {
            if (id_ < rhs.id_)
                return true;
            if (rhs.id_ < id_)
                return false;
            if (x_ < rhs.x_)
                return true;
            if (rhs.x_ < x_)
                return false;
            return y_ < rhs.y_;
        }

        bool operator>(const Point &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const Point &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const Point &rhs) const {
            return !(*this < rhs);
        }

        friend std::ostream &operator<<(std::ostream &os, const Point &point) {
            if (point.id_ != -1) {
                os << point.id_ << ":";
            }
            os << "<" << point.x_ << "," << point.y_ << ">";
            return os;
        }
    };

    /**
     * A bounding box with integer coordinates.
     *
     * The coordinates are inclusive.
     */
    class BoundingBox {
        int min_x_;
        int min_y_;
        int max_x_;
        int max_y_;
    public:
        BoundingBox(int min_x, int min_y, int max_x, int max_y)
                : min_x_(min_x), min_y_(min_y),
                  max_x_(max_x), max_y_(max_y)
        {}

        static BoundingBox from(const std::vector<Point>& points) {
            assert(!points.empty());

            int min_x = points[0].x();
            int max_x = points[0].x();
            int min_y = points[0].y();
            int max_y = points[0].y();

            for (size_t i = 1; i < points.size(); ++i) {
                min_x = std::min(min_x, points[i].x());
                max_x = std::max(max_x, points[i].x());
                min_y = std::min(min_y, points[i].y());
                max_y = std::max(max_y, points[i].y());
            }

            return BoundingBox(min_x, min_y, max_x, max_y);
        }

        static BoundingBox from(const std::vector<BoundingBox>& boxes) {
            assert(!boxes.empty());

            int min_x = boxes[0].min_x();
            int max_x = boxes[0].max_x();
            int min_y = boxes[0].min_x();
            int max_y = boxes[0].max_y();

            for (size_t i = 1; i < boxes.size(); ++i) {
                min_x = std::min(min_x, boxes[i].min_x());
                max_x = std::max(max_x, boxes[i].max_x());
                min_y = std::min(min_y, boxes[i].min_y());
                max_y = std::max(max_y, boxes[i].max_y());
            }

            return BoundingBox(min_x, min_y, max_x, max_y);
        }

        [[nodiscard]] int min_x() const {
            return min_x_;
        }

        [[nodiscard]] int min_y() const {
            return min_y_;
        }

        [[nodiscard]] int max_x() const {
            return max_x_;
        }

        [[nodiscard]] int max_y() const {
            return max_y_;
        }

        [[nodiscard]] int width() const {
            return max_x_ - min_x_;
        }

        [[nodiscard]] int height() const {
            return max_y_ - min_y_;
        }

        [[nodiscard]] bool contains(const Point& point) const {
            return min_x_ <= point.x() && point.x() <= max_x_ &&
                    min_y_ <= point.y() && point.y() <= max_y_;
        }

        [[nodiscard]] bool intersects(const BoundingBox& other) const {
            return !(
                    other.max_x() < min_x_ || max_x_ < other.min_x_ ||
                    other.max_y() < min_y_ || max_y_ < other.min_y_
            );
        }

        [[nodiscard]] BoundingBox bounding_box() const {
            return *this;
        }

        bool operator==(const BoundingBox &rhs) const {
            return min_x_ == rhs.min_x_ &&
                   min_y_ == rhs.min_y_ &&
                   max_x_ == rhs.max_x_ &&
                   max_y_ == rhs.max_y_;
        }

        bool operator!=(const BoundingBox &rhs) const {
            return !(rhs == *this);
        }

        bool operator<(const BoundingBox &rhs) const {
            if (min_x_ < rhs.min_x_)
                return true;
            if (rhs.min_x_ < min_x_)
                return false;
            if (min_y_ < rhs.min_y_)
                return true;
            if (rhs.min_y_ < min_y_)
                return false;
            if (max_x_ < rhs.max_x_)
                return true;
            if (rhs.max_x_ < max_x_)
                return false;
            return max_y_ < rhs.max_y_;
        }

        bool operator>(const BoundingBox &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const BoundingBox &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const BoundingBox &rhs) const {
            return !(*this < rhs);
        }

        friend std::ostream &operator<<(std::ostream &os, const BoundingBox &box) {
            os << "box(" << box.min_x_ << "," << box.min_y_ << " - " << box.max_x_ << "," << box.max_y_ << ")";
            return os;
        }
    };

    /**
     * 
     * @param value The value to approximate
     * @param decimals The number of decimals to keep (supports 0, 1, or 2);
     * @return \a value approximated by using \a decimals fixed precision decimals
     */
    inline int approximate_as_int(const double value, const int decimals = 2) {
        assert(0 <= decimals && decimals <= 2);
        const int scale = decimals == 0 ? 1 : (decimals == 1 ? 10 : 100);
        const int approximate_distance = static_cast<int>(value * scale);
        return approximate_distance;
    }

    /**
     * This function computes the distance between two points, approximating it to an integer by
     * using two fixed decimals.
     *
     * @param start
     * @param end
     * @return The approximate distance between the two points.
     */
    inline int compute_approximate_distance(const Point start, const Point end) {
        const double x_diff = end.x() - start.x();
        const double y_diff = end.y() - start.y();
        const double distance = std::sqrt(x_diff * x_diff + y_diff * y_diff);
        int approximate_distance = approximate_as_int(distance);
        return approximate_distance;
    }

    /**
     * Class representing a line segment between two points inclusive.
     *
     * The length is an approximation, see \a compute_approximate_distance
     */
    class LineSegment {
        Point start_;
        Point end_;
        int length_;
    public:
        LineSegment(const Point &start, const Point &end)
                : start_(start), end_(end),
                  length_(compute_approximate_distance(start_, end_)) {}

        LineSegment(const LineSegment&) = default;
        LineSegment(LineSegment&&) = default;
        LineSegment& operator=(const LineSegment&) = default;
        LineSegment& operator=(LineSegment&&) = default;

        [[nodiscard]] int start_id() const {
            // IDs are 1-based (form the TSPLib format), so reduce by one to get a 0-based index
            return start_.id() - 1;
        }

        [[nodiscard]] int end_id() const {
            // IDs are 1-based (form the TSPLib format), so reduce by one to get a 0-based index
            return end_.id() - 1;
        }

        [[nodiscard]] const Point &start() const {
            return start_;
        }

        [[nodiscard]] const Point &end() const {
            return end_;
        }

        [[nodiscard]] int length() const {
            return length_;
        }

        [[nodiscard]] int id_not(const int node) const {
            if (node == start_id()) {
                return end_id();
            } else {
                assert(node == end_id());
                return start_id();
            }
        }

        [[nodiscard]] BoundingBox bounding_box() const {
            return BoundingBox::from({start_, end_});
        }

        /**
         *
         * @param point
         * @return True iff the point is in the bounding box of this line segment
         */
        [[nodiscard]] bool bounds_contains(const Point& point) const {
            const int x_min = std::min(start_.x(), end_.x());
            const int x_max = std::max(start_.x(), end_.x());
            const int y_min = std::min(start_.y(), end_.y());
            const int y_max = std::max(start_.y(), end_.y());

            return x_min <= point.x() && point.x() <= x_max &&
                    y_min <= point.y() && point.y() <= y_max;
        }

        bool operator==(const LineSegment &rhs) const {
            return start_ == rhs.start_ &&
                   end_ == rhs.end_;
        }

        bool operator!=(const LineSegment &rhs) const {
            return !(rhs == *this);
        }

        bool operator<(const LineSegment &rhs) const {
            if (start_ < rhs.start_)
                return true;
            if (rhs.start_ < start_)
                return false;
            return end_ < rhs.end_;
        }

        bool operator>(const LineSegment &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const LineSegment &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const LineSegment &rhs) const {
            return !(*this < rhs);
        }

        friend std::ostream &operator<<(std::ostream &os, const LineSegment &segment) {
            os << "(" << segment.start_ << "-" << segment.end_ << ")";
            return os;
        }
    };

    /// The summed lengths of a set of lines
    inline int sum_line_lengths(const std::vector<LineSegment> &lines) {
        int result = 0;
        for (const auto &line : lines) {
            result += line.length();
        }
        return result;
    }

    // The following line segment crossing is mostly taken from
    // http://www.dcs.gla.ac.uk/~pat/52233/slides/Geometry1x1.pdf

    /**
     * Type safe orientation enum
     */
    enum class Orientation {
        Colinear,
        Clockwise,
        CounterClockwise,
    };

    /**
     *
     * @param p1
     * @param p2
     * @param p3
     * @return The orientation of the triangle formed by (p1, p2, p3)
     */
    inline Orientation orientation(const Point& p1, const Point& p2, const Point& p3)
    {
        // The base idea is to check the comparison
        // y_diff_1_2 / x_diff_1_2 <=> y_diff_2_3 / x_diff_2_3
        // i.e., the difference in slopes of the first two lines in the triangle
        // However, the x_diff can be 0, so this is a bad idea.
        // This is a closed form comparison formula instead that avoids the issues of division by 0
        const int orientation = (p2.y() - p1.y()) * (p3.x() - p2.x()) -
                                (p3.y() - p2.y()) * (p2.x() - p1.x());

        if (orientation == 0) {
            return Orientation::Colinear;
        } else if (orientation > 0) {
            return Orientation::Clockwise;
        } else { // orientation < 0
            return Orientation::CounterClockwise;
        }
    }

    /**
     *
     * @param l1
     * @param l2
     * @return True iff the line segments intersect
     */
    inline bool intersects(const LineSegment& l1, const LineSegment& l2)
    {
        // Find the four orientations of the triangles formed by the
        // line segments and the others start and end.
        const Orientation o_s1_e1_s2 = orientation(l1.start(), l1.end(), l2.start());
        const Orientation o_s1_e1_e2 = orientation(l1.start(), l1.end(), l2.end());
        const Orientation o_s2_e2_s1 = orientation(l2.start(), l2.end(), l1.start());
        const Orientation o_s2_e2_e1 = orientation(l2.start(), l2.end(), l1.end());

        // General case. If the orientations of the triangles formed by
        // the counterparts are opposites, the segments intersect
        if (o_s1_e1_s2 != o_s1_e1_e2 && o_s2_e2_s1 != o_s2_e2_e1) {
            return true;
        }

        // Special Cases for when some triangles are colinear:
        // If the relevant point of the triangle is not just colinear, but also in the bounds of the line segment,
        // then it is also _on_ the line segment, and thus the lines intersect.
        if (o_s1_e1_s2 == Orientation::Colinear && l1.bounds_contains(l2.start())) {
            return true;
        }
        if (o_s1_e1_e2 == Orientation::Colinear && l1.bounds_contains(l2.end())) {
            return true;
        }
        if (o_s2_e2_s1 == Orientation::Colinear && l2.bounds_contains(l1.start())) {
            return true;
        }
        if (o_s2_e2_e1 == Orientation::Colinear && l2.bounds_contains(l1.end())) {
            return true;
        }

        // No intersection
        return false;
    }
}

#endif //HC_GEOMETRY_H

#pragma clang diagnostic pop
