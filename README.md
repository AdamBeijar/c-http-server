# C-HTTP SERVER

---

### Starting the server

First you need to compile all the files with the `./compile_all.sh` if youre using a unix based system with gcc installed

After that you should have a file in /bin/server that you can start with `./bin/server`

The default server should start on port 8080 and will contain a "/" index page and a "/contact" contact page

---

### Developing wit the backend package

#### Adding views:
To add costume paths/views you need to go to the path.c file and add a new path to the paths[] list. Like {"/path", METHOD, view}
```
path_t paths[] = { 
        {"/", "GET", index_view},
        {"/contact", "GET", contact_view},
        {"/", "POST", index_post_view}
    };
``` 

And then add a view to either the view.c file, you can also create a new view file if you want to and configure it the same.

Adding paths like `{"/path/<int:i>"}` are not supported yet, it will come in a later update

#### Modifying html files dynamically


#### Serving static files:
To serve a static file it needs to be in a directory in the root called "static" and the placed in the correct subfolders.
i.e if you want to serve a css file, it needs to be placed in a /static/css/ and if you want to serve a js file, it needs to be placed in a /static/js/ or a /static/javascript/ folder.
For any other type of file you need to have the first subfolder to be named as the first part of the content-type it is, so for a png it has to be in a /static/image/ folder and a pdf file needs to be in a /static/application/