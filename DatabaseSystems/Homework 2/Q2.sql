/* 
 * Javier Macossay
 * For this problem, I used Oracle Live SQL
 * from the following website: https://livesql.oracle.com/
 */

/*Creating the Table*/
CREATE TABLE ProblemTwo( 
    SID int, 
    ClassName varchar(255), 
    Grade varchar(255) 
);

/*Data Insertion*/
INSERT INTO ProblemTwo (SID, ClassName, Grade) 
VALUES (123, 'ART123', 'A');

INSERT INTO ProblemTwo (SID, ClassName, Grade) 
VALUES (123, 'BUS456', 'B');

INSERT INTO ProblemTwo (SID, ClassName, Grade) 
VALUES (666, 'REL100', 'D');

INSERT INTO ProblemTwo (SID, ClassName, Grade) 
VALUES (666, 'ECO966', 'A');

INSERT INTO ProblemTwo (SID, ClassName, Grade) 
VALUES (666, 'BUS456', 'B');

INSERT INTO ProblemTwo (SID, ClassName, Grade) 
VALUES (345, 'BUS456', 'A');

INSERT INTO ProblemTwo (SID, ClassName, Grade) 
VALUES (345, 'ECO966', 'F');

/*Query Commands*/
/*
 * In this query, I am counting the number of
 * times the class appears in the table, which
 * represents the number of students register
 * to the classes. Then, I am sorting them
 * in ascending order (from smallest to largest
 * value).
 */
SELECT ClassName, COUNT(*) AS Total 
FROM ProblemTwo 
GROUP BY ClassName 
ORDER BY Total ASC;