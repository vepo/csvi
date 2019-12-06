# Thinking and Starting a project

## Reasons
When you start a project, you have to get a clear reason to do it. Today, I'm starting a new project that I will call it **CSV Viewer**. Now I have in mind few objects:

* Create a easy Unix/Linux/Windows command line tool for inspecting CSV
* Remember how is develop using C and Make (old style for me!) 😀
* Use my experience in this project to write useful posts

## Reasoning

Before starting any project, we need to invest some times reasoning about it.

The first decision is selecting what to make: **A CSV Viewer for command line**

The second is selecting what are the minimum features that this project will have.

* It SHOULD make ease read CSV files
* It SHOULD make ease to find values in a CSV files
* It SHOULD have a minimalist interface (vi like)

## Tecnologies

After that whe should select which tecnologies we will use.

I'm selecting C because of the following reasons:

* It is a programming language that I want to keep updated
* It will not required any dependency. No Java, No Node, No Python.... Just the OS if I correctly use stating 

## Breaking the functionalities

The next step is reasoning about what modules your program should have to make it works.

For a minimum CSV Viewer we should have:

| Module | Description | Resposability |
|--------|-------------|---------------|
| CSV Reader | This module SHOULD encapsulate all CSV logic. | Read the CSV File and provide high level functions to access its information |
| UI | This module SHOULD presents all information to the user | Presents all information to the user and receives commands |



## Setupping environment

Before starting coding we need to setup the environment. For that we need to:

1. Install all dependencies to build
2. Install all depenencies to test
3. Select an IDE

So I selected

1. For this project I will use [**Win-builds**](http://win-builds.org/doku.php/start). I would prefer using linux, but now I working in a Windows Environment, so it is easy to emulate a linux environment than starting developing C in Windows. I will use Make and GCC and the makefine SHOULD be just one for Linux and Windows.
2. 