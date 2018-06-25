#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

// clang-format off
template < typename... Args > void eater( Args... ) {}
// clang-format on

namespace logger
{
    template < typename... Args > void log( Args&&... args )
    {
        const auto now = std::chrono::high_resolution_clock::now();

        std::cout << "\033[1;32m";
        std::cout << "[" << now.time_since_epoch().count() << "] ";
        eater( ( ( std::cout << args ), 0 )... );
        std::cout << "\033[1;0m";
        std::cout << std::endl;
    }

#ifdef DEBUG
    template < typename... Args > void log_debug( Args&&... args )
    {
        log( std::forward< Args >( args )... );
    }
#else
    void log_debug( ... )
    {
    }
#endif
} // namespace logger

using polygon_data_type = std::vector< glm::vec3 >;

void log_polygon( const polygon_data_type& polygon );

template < int32_t width, int32_t height, int32_t depth > struct square_polygon_generator
{
    template < typename T > static constexpr auto indices_count = T{4};
    static constexpr auto origin = glm::vec3{0.0f, 0.0f, 0.0f};
    static constexpr auto size   = glm::vec3{width, height, depth};
};

template < typename T > polygon_data_type generate_polygon( T&& ) noexcept
{
    constexpr auto indices_count = T::template indices_count< int32_t >;
    polygon_data_type ret( indices_count, glm::vec3{0.0f, 0.0f, 0.0f} );

    constexpr bool is_odd = ( indices_count & 1 ) == 0;
    const glm::vec3 first_point =
        is_odd ? T::origin - ( T::size * glm::vec3( 0.5f ) )
               : T::origin - ( T::size * glm::vec3( 0.0f, 0.5f, 0.0f ) );

    const auto angle =
        ( glm::pi< float >() * 2.0f ) / ( T::template indices_count< float > );

    // set first point
    ret[0] = first_point;

    for ( int32_t i = 1; i < indices_count; ++i )
    {
        const auto angle_r = angle * i;
        ret[i]             = glm::rotateZ( first_point, angle_r );
    }

    return ret;
}

glm::vec3 get_edge( const polygon_data_type& polygon, size_t i0, size_t i1 )
{
    return polygon[i1] - polygon[i0];
}

polygon_data_type calculate_edges( const polygon_data_type& polygon )
{
    polygon_data_type ret( polygon.size(), glm::vec3( 0.0f ) );

    for ( auto i = 0u; i < polygon.size() - 1; ++i )
    {
        ret[i] = get_edge( polygon, i, i + 1 );
    }

    ret[polygon.size() - 1] = get_edge( polygon, polygon.size() - 1, 0 );

    return ret;
}

template < typename T, typename F >
std::vector< T > for_each_element( std::vector< T >&& data, F&& f )
{
    for ( auto& e : data )
    {
        std::forward< F >( f )( e );
    }

    return data;
}

float calculate_angle( const glm::vec3& v0, const glm::vec3& v1 )
{
    const auto vsin = glm::dot( glm::cross( v0, v1 ), glm::vec3( 0.0f, 0.0f, 1.0f ) );
    const auto vcos = glm::dot( v0, v1 );

    logger::log_debug( "vsin: ", glm::degrees( glm::asin( vsin ) ),
                       " vcos: ", glm::degrees( glm::acos( vcos ) ) );

    const auto angle = glm::atan( vsin, vcos );

    logger::log_debug( "raw angle: ", angle );
    return angle < 0.0f ? angle + glm::pi< float >() * 2.0f : angle;
}

std::vector< float > calculate_angles( const polygon_data_type& polygon )
{
    std::vector< float > ret( polygon.size(), 0.0f );

    const polygon_data_type normalized_edges = for_each_element(
        calculate_edges( polygon ), []( glm::vec3& e ) { e = glm::normalize( e ); } );
    log_polygon( normalized_edges );
    ret[0] = calculate_angle( -normalized_edges[normalized_edges.size() - 1],
                              normalized_edges[0] );

    for ( auto i = 1u; i < normalized_edges.size(); ++i )
    {
        const auto e0    = normalized_edges[i - 1];
        const auto e1    = normalized_edges[i];
        const auto angle = calculate_angle( -e0, e1 );
        ret[i]           = angle;
    }

    return ret;
}

void log_polygon( const polygon_data_type& polygon )
{
    int32_t counter = 0;
    for ( const auto& e : polygon )
    {
        logger::log( "[", counter++, "] - ", "x: ", e.x, " y: ", e.y, " z: ", e.z );
    }
}

int main()
{
    polygon_data_type polygon =
        generate_polygon( square_polygon_generator< 20, 10, 0 >{} );
    log_polygon( polygon );

    logger::log( "Edges: " );

    polygon_data_type polygon_edges = calculate_edges( polygon );
    log_polygon( polygon_edges );

    const std::vector< float > angles = calculate_angles( polygon );
    for ( const auto& a : angles )
    {
        logger::log( glm::degrees( a ) );
    }

    return 0;
}
