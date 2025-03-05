Please read the following instructions carefully. Failure to follow the instructions will results in losing at least 5 points.

This assignment builds on Week5assignment 2, if you worked in a team, you should continue to do so. If you worked individually on assignment 2, You may continue to work individually on this assignment or in groups of at most two students. If you choose to work as group (no more than two students), you should let me know before you form a team. Here are the general rules for teams:
You should work on the whole assignment together.
You should not just partition the work between you.
You should upload only one submission to canvas.
You should form the pair on Canvas prior to the deadline.
Hand in all program header/source files softcopy using Canvas and be sure that they properly execute. Failure to do so will mean that your program is not graded.
All header/source files should be compressed into a ZIP archive using the following naming format: zip or firstname1_lastname1_firstname2_lastname2_asmt2.zip for groups.
Your ZIP file should include: (1) the folder LinkedBagDS, (2) cpp, and (3) all the classes .cpp and .h files.
Please type your answer for parts 4 and hand it in a PDF file through Canvas. Do not include it in the ZIP
Please include your full name(s) on the PDF.
Please refer to the university integrity policy on the syllabus, and remember that you should be able to explain and reproduce any code you write and submit as part of this assignment.
 

ASSIGNMENT GOALS AND OBJECTIVES

Programming methodology:
Analyze updated problem requirements.
Implement program maintenance by enhancing assignment 2 implementation for assignment 3.
C++ implementation:
Implement operator overload functions for user-defined data types.
Use of smart pointers for better management of dynamic memory allocation.
Analyze the linkedBag implementation of ADT Bag (similar to LinkedList).
Implement the overload assignment operator function in the provided LinkedBag implementation.
PROBLEM DEFINITION

You will enhance your implementation of the simplified version of Instagram. And to do so, you will implement the following features:

use smart pointers instead of raw pointers to manage the list of posts and users,
add operator overloading for specific operators, and
add the implementation of the BIG 3 functions in your program classes. For extra credit, you may choose to do one or more of the following:
Add two types of users: Personal and Creator.
Allow users to either “sign up” by creating an account or “log in” by retrieving their account from the list of users.
Add the overload Assignment (=) Operator to the linked-list implementation of the ADT Bag (LinkedBag).
Submit one non-trivial test cases for the new implementation of Instagram340.
APPLICATION REQUIREMENTS — INSTAGRAM340

Your client is happy with your product and would like to enhance it with new features. Your application will still only implement two main aspects: managing users and managing their posts. You do not have to implement any features pertaining to messaging, following other users, or reacting to posts.

Your application should still allow a user to perform the tasks described in assignment 2 handout. In addition, you should implement the following 5 parts.

PART 0: ADD MENU OPTION — 5 points

You may use the same main function that displays the menu. Your task is to add option 8, which allows the user to edit the post.

PART 1: SMART POINTERS — 10 points

There are two lists of objects in your program: a list of User objects and a list of Post objects. In order to take advantage of polymorphism, your lists should hold pointers to objects. Because the LinkedBag does not help manage dynamically allocated items, you should use smart pointers. Choose the right type of smart pointers and use them in your new implementation of the application.

PART 2: FRIEND FUNCTIONS — 10 points

Your program prompts the user for information to (1) initialize a User profile and (2) initialize Posts (both Reel and Story). Your program also displays user profile information and posts. You should overload both output operator << and input operator >> in all classes to perform these tasks.

In Instagram340, you only need to overload the output operator << to display the following message: “Welcome to Instagram 340!”

PART 3: BIG 3 — 20 points

You should implement the BIG 3 functions. The BIG 3 are three of the BIG 5 functions: the destructor, the copy constructor, and overload assignment operator. You should include an implementation in all program classes.

PART 4: DESIGN DECISIONS – 5 points

Thinking about Instagram340, which data structure(s) do you think would be most efficient for it (time and space efficiency). In your analysis you should think about the main points discussed in class: (1) data volume, (2) operation frequency (how frequently do you do searches), (3) data growth (how fast does data grow), and (4) search needs (how is data retrieved? simple vs. complex queries).

 

EXTRA CREDIT

PART EC1: NEW TYPES OF USERS — 2 points

There are different types of users. In this iteration, we want to add two types of users: Personal and Creator. You have the liberty to design these types of users, but they must have at least one feature that is different, and which would be a good example of polymorphism.

PART EC2: SIGN UP OR  LOG IN — 2 points

The application starts by asking the user to create an account. Once the account is created, it is added to the list of users. Your task is to add an option to the main menu allowing the user to either create an account or to retrieve their account. It may be as simple as finding the kth user account using the function you have implemented in assignment 2.

With the new addition, when a user logs out (option 0), the program should not exit. It should instead redirect them to the Instagram340 landing page, which is just a menu for creating an account, logging in, or exiting the application.

PART EC3: LINKEDBAG OPERATOR(=) OVERLOADING — 2 points

You will use the same LinkedBag implementation of the interface for Bag ADT provided to you as part of assignment 2 in the folder LinkedBagDS.

Add the implementation of the following function:

LinkedBag<ItemType>& operator=(const LinkedBag<ItemType>& aBag);
It is very similar to the copy constructor (already provided in the current implemen­tation), except you will be modifying the this object.

You need to first check if the this is the same as aBag. If not, you need to clear this, i.e. if (this != &aBag) you need to clear the this object.

PART EC4: NON-TRIVIAL TEST CASE — 3 points

You should provide one non-trivial test case to show that you have tested your program for correctness. Your test case should include sample inputs and expected outputs.

Sample input could be a sequence of menu options a user would choose
Sample output would be the expected behavior of the program or next step Save the sample inputs in txt and its expected output in output01.txt.
Include it in your ZIP file
