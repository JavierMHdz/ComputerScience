/* 
 * Javier Macossay
 * For this problem, I used Oracle Live SQL
 * from the following website: https://livesql.oracle.com/
 */

/*Creating the Table*/
Create Table ProblemThree( 
    ProjectID varchar(255), 
    Step int, 
    Status varchar(255) 
);

/*Data Insertion*/
INSERT INTO PROBLEMTHREE(ProjectID, Step, Status) VALUES('P100',2,'W');
INSERT INTO PROBLEMTHREE(ProjectID, Step, Status) VALUES('P100',1,'W');
INSERT INTO PROBLEMTHREE(ProjectID, Step, Status) VALUES('P201',0,'C');
INSERT INTO PROBLEMTHREE(ProjectID, Step, Status) VALUES('P100',0,'C');
INSERT INTO PROBLEMTHREE(ProjectID, Step, Status) VALUES('P201',1,'C');
INSERT INTO PROBLEMTHREE(ProjectID, Step, Status) VALUES('P333',0,'W');
INSERT INTO PROBLEMTHREE(ProjectID, Step, Status) VALUES('P333',1,'W');
INSERT INTO PROBLEMTHREE(ProjectID, Step, Status) VALUES('P333',2,'W');
INSERT INTO PROBLEMTHREE(ProjectID, Step, Status) VALUES('P333',3,'W');

/*Query Commands*/
/*
 * I am getting to tables, one that is the result of step = 0 and
 * status = 'c' and the second table is the result of step = 1 and
 * status = 'w'. Then, I am finding the intersection (the rows that are
 * in common) from both tables. Thus, finding the desired output.
 */
Select ProjectID 
From ProblemThree 
Where Step = 0 AND Status = 'C' 
INTERSECT 
Select ProjectID 
From ProblemThree 
Where Step = 1 AND Status = 'W';