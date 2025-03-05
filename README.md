San Francisco State University
CSC 317 – Introduction to Web Development Software
Assignment Two
Introduction
For assignment two, you will be implementing a few pages in HTML ONLY. No other languages or
technologies can be used. This will come later. The purpose of doing only HTML is to get familiar with
HTML and its syntax and to focus only on structure and semantic meaning, HTML’s main purpose. When
designing these pages, more specifically, the ones that contain forms, think about what the required
data is and how they are related. The HTML pages that are being implemented in Assignment two will
be used in your final project. The pages that will be implemented are index.html, login.html,
registration.html, postvideo.html, and viewpost.html.
Setup
Please use the link on Canvas to create your repository for the next few assignments.
Please create a new branch in your repo named “assignment2”. This branch should be created off the
main branch. When the assignment is completed, you will merge the assignment2 branch back onto
main. Please DO NOT delete branches when assignments are completed.
A new branch can be created with the following command:
git checkout -b assignment2
If you want to publish the branch to GitHub, you can do the following command:
git push origin assignment2
Requirements
Students are required to implement five pages in HTML ONLY per the specifications below:
❖ Index.html – Landing page for application
o Must have a banner (pager header, doesn’t need to be an image can be just text)
o Must have a nav bar.
▪ Must have nav bar ON EVERY page
▪ Must have each link working on each page. No dead-end pages.
o Must have a section for showing main page content
o Must have a footer.
[ More content below]
❖ login.html – for users to log in
o Must have document title
o Must have a section heading introducing the form
o Must have a nav bar.
o Input fields are REQUIRED to be in form tags.
o Must ask user to input a username
o Must ask user to input a password.
▪ Password input must be correct input type (i.e. when I type I should not see the
password)
o Must have the user click a submit button to submit login form.
❖ registration.html – for guests to register accounts
o Must have a document title
o Must have a section heading introducing the form
o Must have a nav bar
o Input fields are REQUIRED to be in form tags.
o Must ask user to input a username
o Must ask user to input an email.
o Must ask user to input a password
▪ Password input must be correct input type (i.e. when I type I should not see the
password)
o Must ask user to input a confirm password
▪ Password input must be correct input type (i.e. when I type I should not see the
password)
o Must ask user to accept they are 13+ years of age.
o Must ask users to accept TOS and Privacy rules
▪ Simply make a link that goes nowhere, but the text says TOS and Privacy Rules.
o Must have the user click a submit button to submit registration form.
❖ postvideo.html – for registered users to post videos
o Must have document title
o Must have a section heading introducing the form
o Must have nav bar
o Input fields are REQUIRED to be in form tags.
o Must ask user to input a post Title
o Must ask user to input a post Description
o Must ask user to input a video (we will only stick to video formats that are support by
the HTML5 player.)
o Must ask user to accept Acceptable Use Policy for uploading videos.
o Must ask user to submit video post via a button.
❖ viewpost.html – for viewing/watching an individual video post
o Must have a nav bar
o Must have a tag for the post’s post title.
o Must have a tag for the post’s post author
o Must have a tag for the post’s post creation time.
o Must have a tag for the post’s video
o Must have a tag for the post’s comment section for the video.
Committing Changes
git add .
git commit -m “some message”
git push origin assignment2
Note if you are not on the main branch then replace main in the above push command with the correct
branch name. You can see the branch name using the git status command.
Merge Changes into Main
While on the assignment2 branch, please verify all your changes are on GitHub. You can do this by going
to your repository on GitHub and browsing your repository looking for the files.
Once you have verified your changes are present on GitHub, switch to the main branch on your local
repository (this is on your computer).
git checkout main
Once switched execute the following command:
git merge assignment2
This will most likely produce a fast-forward, meaning no new commits will be made. Once the merge is
completed, push the updated branch to GitHub with the following commands:
git push origin main
[ More Content Below ]
Submission
When you are done with the assignment please do the following:
❖ Save all work done.
❖ Commit all html pages to the assignment2 branch.
❖ Push new commits to GitHub (these should be on the assignment2 branch NOT main).
❖ Verify the commits are pushed to GitHub by browsing your repo in your favorite browser.
❖ Once verified, merge all the work done from your assignment2 branch to main.
❖ Then push your main branch to GitHub.
Grading
Your assignment we be graded on the following criteria’s:
❖ Structure, how we organized are your forms.
o Note since these are HTML only pages the visual look won’t be appealing but this is OK.
❖ Correct usage of HTML tags.
❖ Completion of all requirements.
❖ Completed merge from assignment2 branch to main branch.

