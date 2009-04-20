/*
Layout:
/posts/ - Contains all the posts for the blog. 
- The way they are named is the rewrite url form.
- use chmod to hold the permissions of who can read and write the file
- if chmod +or -> then it is published
- if chmod -or -> then it is a draft.
- if chmod +gr -> then pending review         
#
/categories/:name/
- :name is the category name
- each category has a symbolic link to the posts in /posts/
- if a post's link is destroyed that category is removed from the post.
/pages/
- should have a list of pages.
- subpages should be in directorys with the same name as the parent page.
/comments/(hold|approve|spam)
- hold is non-moderated comments
- approve is approved comments. Move hold comments to approve to approve the comments
- Move comments to spam to mark them as spam.
/uploads/
- Upload files here. 
- A .meta file should be created for each uploaded file.
*/

/*
   self.blog_url = blog_url
   self.username = username
   self.password = password

   self.root = '.'

# Connect to the xml-rpc
# http://codex.wordpress.org/XML-RPC 
xmlrpc = self.blog_url + '/xmlrpc.php'
self.client = xmlrpclib.ServerProxy(xmlrpc)

# Load posts
my_blogs = self.client.wp.getUsersBlogs(username, password)

# url, isAdmin, blogid, xmlrpc, blogName
self.blog_info = my_blogs[0]

# Find the blog we are looking for based on blog_url if there is more than one blog.
if len(my_blogs) > 1:
for blog in my_blogs:
if self.blog_url in blog['url']:
self.blog_info = blog

self.posts = self.client.metaWeblog.getRecentPosts(self.blog_info['blogid'], username, password, '0')

self.files = {}

for post in self.posts:
self.files[self.root + '/' + post['wp_slug']] = {
'st_mode': 0755,
'st_uid': os.getuid(),
'st_gid': os.getgid(),
'st_mtime': time(), 
'st_atime': time()
}


self.files[self.root + '/'] = {
'st_mode': 0755,    # If file is pending +grw if pubish +orw
'st_ino': '0',      # inode number - is the post id.

#      'st_size':  (size of file, in bytes)
'st_uid': os.getuid(),
'st_gid': os.getgid(),
'st_nlink': len(self.posts) + 1,
'st_ctime': time(), 
'st_mtime': time(), 
'st_atime': time()
}

*/

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>

#include "wpapi.h"

static const char *wpfs_str = "Hello World!\n";
static const char *wpfs_path = "/wpfs";

static struct wp_post *posts;

static int wpfs_getattr(const char *path, struct stat *stbuf)
{
    int i;
    struct wp_post *post = posts;

    syslog(LOG_INFO, "In getattr\n");

    memset(stbuf, 0, sizeof(struct stat));

    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();

    if(strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = postslen(posts) + 1;

        syslog(LOG_INFO, "In getattr path = %s \n", path);
        return 0;
    }
    
    /* Find slug. */
    for(i = 0; i < postslen(posts); i++) {
        if(strcmp(path, post->slug_path) == 0) {
            stbuf->st_mode = S_IFREG | 0664;
            stbuf->st_nlink = 1;

            /* FIXME: Get the actual size eventually */
            stbuf->st_size = 100; 

            syslog(LOG_INFO, "In getattr path = %s \n", path);
            return 0;
        }

        post++;
    }

    return -ENOENT;
}

static int wpfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi)
{
    int i;
    struct wp_post *post = posts;

    (void) offset;
    (void) fi;

    syslog(LOG_INFO, "In readdir\n");

    if(strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for(i = 0; i < postslen(posts); i++) {
        filler(buf, post->slug_path + 1, NULL, 0);
        syslog(LOG_INFO, "In readdir added path = %s \n", post->slug_path);

        post++;
    }

    return 0;
}

static int wpfs_open(const char *path, struct fuse_file_info *fi)
{

    syslog(LOG_INFO, "In open\n");

    /*
    if(strcmp(path, wpfs_path) != 0)
        return -ENOENT;
    */
    /*
    if((fi->flags & 3) != O_RDONLY)
        return -EACCES;
    */

    return 0;
}

static int wpfs_read(const char *path, char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;

    syslog(LOG_INFO, "In read\n");

    if(strcmp(path, wpfs_path) != 0)
        return -ENOENT;

    len = strlen(wpfs_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, wpfs_str + offset, size);
    } else
        size = 0;

    return size;
}

static void wpfs_destroy(void)
{
    wp_destroy(posts);
}

static struct fuse_operations wpfs_oper = {
    .getattr  = wpfs_getattr,
    .readdir  = wpfs_readdir,
    .open = wpfs_open,
    .read = wpfs_read,
    .destroy = wpfs_destroy
};

int main(int argc, char *argv[])
{
    wp_init(posts);

    printf("Len: %d\n", postslen(posts));

    return fuse_main(argc, argv, &wpfs_oper, NULL);
}
