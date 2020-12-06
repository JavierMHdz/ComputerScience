/* 
 * Javier Macossay
 * For this problem, I used Oracle Live SQL
 * from the following website: https://livesql.oracle.com/
 */

/*Creating the Table*/
CREATE TABLE ProblemFive( 
    Chef varchar(255), 
    Dish varchar(255) 
);

/*Data Insertion*/
INSERT INTO ProblemFive (Chef, Dish) VALUES ('A','Mint chocolate brownie');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('B','Upside down pineapple cake');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('B','Creme brulee');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('B','Mint chocolate brownie');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('C','Upside down pineapple cake');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('C','Creme brulee');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('D','Apple pie');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('D','Upside down pineapple cake'); 
INSERT INTO ProblemFive (Chef, Dish) VALUES ('D','Creme brulee');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('E','Apple pie');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('E','Upside down pineapple cake');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('E','Creme brulee');
INSERT INTO ProblemFive (Chef, Dish) VALUES ('E','Bananas Foster');

/*Query Commands*/
/*
 * This version of problem 5 uses a subquery. In the subquery, I select the rows
 * that have the chefs who can prepare either of the three options of the dishes.
 * Then, return those chefs to the outer query and group them by chef and only
 * only select the chefs that appear on the table three times because that is the
 * desired amount of dishes.
 */
SELECT Chef 
FROM (SELECT * 
      FROM ProblemFive 
      WHERE Dish = 'Apple pie' OR Dish = 'Upside down pineapple cake' OR Dish = 'Creme brulee') 
GROUP BY Chef 
HAVING Count(Chef) = 3;