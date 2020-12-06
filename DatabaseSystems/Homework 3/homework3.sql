/*Creating the table*/
CREATE TABLE POINTS(
  location_id integer NOT NULL primary key,
  location_name varchar(255) NOT NULL,
  location geometry(point,4326) NOT NULL
);

/*Inserting the points*/
INSERT INTO POINTS(location_id, location_name, location)
VALUES
(1, 'My House', ST_PointFromText('POINT(-118.2929888 34.0241559)',4326)),
(2, 'Jefferson and Vermont', ST_PointFromText('POINT(-118.2911592 34.0253422)',4326)),
(3, 'Lyon Center Pool', ST_PointFromText('POINT(-118.2883677 34.0242560)',4326)),
(4, 'Engineering Quad', ST_PointFromText('POINT(-118.2889722 34.0206082)',4326)),
(5, 'SGM Building', ST_PointFromText('POINT(-118.2888873 34.0214060)',4326)),
(6, 'Financial Aid Office', ST_PointFromText('POINT(-118.2840188 34.0195906)',4326)),
(7, 'El Centro Chicano', ST_PointFromText('POINT(-118.2858652 34.0201330)',4326)),
(8, 'Doheny Library', ST_PointFromText('POINT(-118.2836148 34.0198323)',4326)),
(9, 'Leavey Library', ST_PointFromText('POINT(-118.2828845 34.0216141)',4326)),
(10, 'Alumni House', ST_PointFromText('POINT(-118.2828343 34.0194480)',4326)),
(11, 'Architecture School', ST_PointFromText('POINT(-118.2878607 34.0192557)',4326)),
(12, 'Parkside Dorms', ST_PointFromText('POINT(-118.2909345 34.0190081)',4326));

/*Getting Polygon*/
SELECT ST_AsText(ST_ConvexHull(ST_Collect(points.location))) FROM points;

/*Getting Four Nearest Neighbors*/
SELECT ST_AsText(points.location)
FROM points
WHERE points.location::Point <> ST_PointFromText('POINT(-118.2929888 34.0241559)',4326)::Point
ORDER BY ST_Distance(ST_PointFromText('POINT(-118.2929888 34.0241559)',4326), points.location)
LIMIT 4;