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
 * In this query, I am finding three tables that return as a result the chefs
 * that can prepare the desired three dishes, which are hard coded. Then,
 * I create an intersect of the three tables to find the chefs that can make
 * the three dishes.
 */
Select Chef 
From ProblemFive 
Where Dish = 'Apple pie' 
 
Intersect 
 
Select Chef 
From ProblemFive 
Where Dish = 'Creme brulee' 
 
Intersect 
 
Select Chef 
From ProblemFive 
Where Dish = 'Upside down pineapple cake';