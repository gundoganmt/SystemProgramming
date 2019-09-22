//Alper Karayaman 150140016
//Mahmut Gundogan 150110024

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define NUMBER_OF_ROWS 36307
#define LENGTH 50
#define LINE_LENGTH 1024
FILE * file;
struct row{
	char code[LENGTH];
	char neighborhood[LENGTH];
	char city[LENGTH];
	char district[LENGTH];
	char latitude[LENGTH];
	char longitude[LENGTH];
}rows[NUMBER_OF_ROWS];

void parse(int row_index,char *line){
	int i;
	int len=strlen(line);
	int line_pos=0;
	int element_pos=0;
	int column_index=0;
	while(line[line_pos] != '\n' && line_pos < len){
		if(line[line_pos] == '\t'){
			if(column_index == 0){
				rows[row_index].code[element_pos]='\0';
			}
			else if(column_index == 1){
				rows[row_index].neighborhood[element_pos]='\0';
			}
			else if(column_index == 2){
				rows[row_index].city[element_pos]='\0';
			}
			else if(column_index == 3){
				rows[row_index].district[element_pos]='\0';
			}
			else if(column_index == 4){
				rows[row_index].latitude[element_pos]='\0';
			}
			else if(column_index == 5){
				rows[row_index].longitude[element_pos]='\0';
			}
			column_index++;
			element_pos=0;
		}
		else{
			if(column_index == 0){
				rows[row_index].code[element_pos]=line[line_pos];
			}
			else if(column_index == 1){
				rows[row_index].neighborhood[element_pos]=line[line_pos];
			}
			else if(column_index == 2){
				rows[row_index].city[element_pos]=line[line_pos];
			}
			else if(column_index == 3){
				rows[row_index].district[element_pos]=line[line_pos];
			}
			else if(column_index == 4){
				rows[row_index].latitude[element_pos]=line[line_pos];
			}
			else if(column_index == 5){
				rows[row_index].longitude[element_pos]=line[line_pos];
			}
			element_pos++;
		}
		line_pos++;
	}
}

static int do_getattr( const char *path, struct stat *st )
{
	int i,j;
	int count;
	int pos=5;
	int path_length=strlen(path);
	st->st_uid = getuid(); 
	st->st_gid = getgid();
	st->st_atime = time( NULL );
	st->st_mtime = time( NULL );
	
	if ( strcmp( path, "/" ) == 0 )
	{
		st->st_mode = S_IFDIR | 0777;
		st->st_nlink = 2; 
	}
	else if ( strcmp( path, "/NAME" ) == 0 )
	{
		st->st_mode = S_IFDIR | 0777;
		st->st_nlink = 82; 
	}
	else if ( strcmp( path, "/CODE" ) == 0 )
	{
		st->st_mode = S_IFDIR | 0777;
		st->st_nlink = 1; 
	}
	else
	{	
		if(path_length > 5 && path[path_length-4] == '.'){
			st->st_mode = S_IFREG | 0777;
			st->st_nlink = 1;
			st->st_size = 100;
			if(strncmp(path,"/NAME",pos) == 0){
				pos=path_length-5;
				while(path[pos] != '/') pos--;
				pos++;
				for(i=0;i<NUMBER_OF_ROWS;i++){
					j=0;
					while(path[pos+j] != '.' && rows[i].neighborhood[j] != '\0' && path[pos+j] == rows[i].neighborhood[j]){
						j++;
					}
					if(path[pos+j] == '.' && rows[i].neighborhood[j] == '\0'){
						st->st_size=sizeof(char)*(1+strlen(rows[i].code)+strlen(rows[i].city)+strlen(rows[i].district)+strlen(rows[i].neighborhood)
						+strlen(rows[i].latitude)+strlen(rows[i].longitude)+63);
						break;
					}
				}
			}
			else if(strncmp(path,"/CODE",pos) == 0){
				pos=path_length-5;
				while(path[pos] != '/') pos--;
				pos++;
				for(i=0;i<NUMBER_OF_ROWS;i++){
					j=0;
					while(path[pos+j] != '.' && rows[i].neighborhood[j] != '\0' && path[pos+j] == rows[i].code[j]){
						j++;
					}
					if(path[pos+j] == '.' && rows[i].neighborhood[j] == '\0'){
						st->st_size=sizeof(char)*(1+strlen(rows[i].code)+strlen(rows[i].city)+strlen(rows[i].district)+strlen(rows[i].neighborhood)
						+strlen(rows[i].latitude)+strlen(rows[i].longitude)+63);
						break;
					}
				}
			}
			else return -ENOENT;
		}
		else{
			st->st_mode = S_IFDIR | 0777;
			st->st_nlink = 1;
		}
	}
	return 0;
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
	char temp_code[3]="  ";
	int i,pos=5,start_index;
	char* temp_path="";
	char for_txt[LENGTH+20];	
	filler( buffer, ".", NULL, 0 ); 
	filler( buffer, "..", NULL, 0 );
	if ( strcmp( path, "/" ) == 0 )
	{
		filler( buffer, "NAME", NULL, 0 );
		filler( buffer, "CODE", NULL, 0 );
	}
	else if(strncmp(path,"/NAME",pos) == 0){
		if(path[pos] == '\0'){
			for(i=0;i<NUMBER_OF_ROWS;i++){
				if(strcmp(temp_path,rows[i].city) != 0){
					filler( buffer, rows[i].city, NULL, 0 );
					temp_path=rows[i].city;
				}
			}
		}
		else{
			pos++;
			start_index=pos;
			while(1){
				if(path[pos] == '\0') break;
				if(path[pos] == '/') break;
				pos++;
			}
			if(path[pos] == '\0'){ 
				for(i=0;i<NUMBER_OF_ROWS;i++){
					if(strcmp(path+start_index,rows[i].city) == 0){
						if(strcmp(temp_path,rows[i].district) != 0){
							filler( buffer, rows[i].district, NULL, 0 );
							temp_path=rows[i].district;
						}
					}
				}
			}
			else{
				pos++;
				start_index=pos;
				for(i=0;i<NUMBER_OF_ROWS;i++){
					if(strcmp(path+start_index,rows[i].district) == 0){
						strcpy(for_txt,rows[i].neighborhood);
						strcat(for_txt,".txt");
						filler( buffer, for_txt, NULL, 0 );
					}
				}
			}
		}
	}
	else if(strncmp(path,"/CODE",pos) == 0){
		if(path[pos] == '\0'){
			for(i=0;i<NUMBER_OF_ROWS;i++){
				temp_code[0]=rows[i].code[0];
				temp_code[1]=rows[i].code[1];
				if(strncmp(temp_path,temp_code,2) != 0){
					filler( buffer, temp_code, NULL, 0 );
					temp_path=rows[i].code;
				}
			}
		}
		else{
			pos++;
			start_index=pos;
			while(1){
				if(path[pos] == '\0') break;
				if(path[pos] == '/') break;
				pos++;
			}
			if(path[pos] == '\0'){ 
				for(i=0;i<NUMBER_OF_ROWS;i++){
					temp_code[0]=rows[i].code[0];
					temp_code[1]=rows[i].code[1];
					if(strncmp(path+start_index,temp_code,2) == 0){
						if(strcmp(temp_path,rows[i].code) != 0){
							strcpy(for_txt,rows[i].code);
							strcat(for_txt,".txt");
							filler( buffer, for_txt, NULL, 0 );
							temp_path=rows[i].code;
						}
					}
				}
			}
			else return -ENOENT;
		}
	}
	else return -ENOENT;
	return 0;
}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{	
	char contents[LINE_LENGTH];
	char arr[4][LENGTH];
	int i;
	int arr_pos=0;
	int w_pos=0;
	int pos=1;
	while(path[pos] != '.'){
		if(path[pos] == '/'){
			arr[arr_pos][w_pos]='\0';
			w_pos=0;
			arr_pos++;
		}
		else{
			arr[arr_pos][w_pos]=path[pos];
			w_pos++;
		}
		pos++;
	}
	contents[0]='\0';
	if(strcmp(arr[0],"NAME") == 0){
		for(i=0;i<NUMBER_OF_ROWS;i++){
			if((strcmp(arr[1],rows[i].city) == 0) && (strcmp(arr[2],rows[i].district) == 0) && (strcmp(arr[3],rows[i].neighborhood) == 0)){
				strcat(contents,"code: ");
				strcat(contents,rows[i].code);
				strcat(contents,"\n");
				strcat(contents,"neighborhood: ");
				strcat(contents,rows[i].neighborhood);
				strcat(contents,"\n");
				strcat(contents,"city: ");
				strcat(contents,rows[i].city);
				strcat(contents,"\n");
				strcat(contents,"district: ");
				strcat(contents,rows[i].district);
				strcat(contents,"\n");
				strcat(contents,"latitude: ");
				strcat(contents,rows[i].latitude);
				strcat(contents,"\n");
				strcat(contents,"longitude: ");
				strcat(contents,rows[i].longitude);
				strcat(contents,"\n");
				break;
			}
		}
	}
	else if(strcmp(arr[0],"CODE") == 0){
		for(i=0;i<NUMBER_OF_ROWS;i++){
			if(strcmp(arr[2],rows[i].code) == 0){
				strcat(contents,"code: ");
				strcat(contents,rows[i].code);
				strcat(contents,"\n");
				strcat(contents,"neighborhood: ");
				strcat(contents,rows[i].neighborhood);
				strcat(contents,"\n");
				strcat(contents,"city: ");
				strcat(contents,rows[i].city);
				strcat(contents,"\n");
				strcat(contents,"district: ");
				strcat(contents,rows[i].district);
				strcat(contents,"\n");
				strcat(contents,"latitude: ");
				strcat(contents,rows[i].latitude);
				strcat(contents,"\n");
				strcat(contents,"longitude: ");
				strcat(contents,rows[i].longitude);
				strcat(contents,"\n");
				break;
			}
		}
	}
	if(contents[0] == '\0')
		return -ENOENT;
	memcpy( buffer, contents + offset, size );
	return strlen( contents ) - offset;
}

static int do_rename(const char *old_path,const char *new_path){
	char bigpath[1024];
	char newbigpath[1024];
	char old_arr[4][LENGTH];
	char new_arr[4][LENGTH];
	int arr_pos=0;
	int w_pos=0;
	int pos=1;
	int i;
	int flag=0;
	while(old_path[pos] != '.'){
		if(old_path[pos] == '/'){
			old_arr[arr_pos][w_pos]='\0';
			w_pos=0;
			arr_pos++;
		}
		else{
			old_arr[arr_pos][w_pos]=old_path[pos];
			w_pos++;
		}
		pos++;
	}
	arr_pos=0;
	w_pos=0;
	pos=1;
	while(new_path[pos] != '.'){
		if(new_path[pos] == '/'){
			new_arr[arr_pos][w_pos]='\0';
			w_pos=0;
			arr_pos++;
		}
		else{
			new_arr[arr_pos][w_pos]=new_path[pos];
			w_pos++;
		}
		pos++;
	}
	if(strcmp(old_arr[0],"NAME") == 0){
		for(i=0;i<NUMBER_OF_ROWS;i++){
			if((strcmp(old_arr[1],rows[i].city) == 0) && (strcmp(old_arr[2],rows[i].district) == 0) && (strcmp(old_arr[3],rows[i].neighborhood) == 0)){
				strcpy(rows[i].neighborhood,new_arr[3]);
				flag=1;
				break;
			}
		}
	}
	if(flag){
		strcpy(bigpath,"/home/lubuntu/Desktop/project3/folderforfuse");
		strcpy(newbigpath,"/home/lubuntu/Desktop/project3/folderforfuse");
		strcat(bigpath,old_path); 
		strcat(newbigpath,new_path);
		rename(bigpath,newbigpath);
		return 0;
	}
	else return -1;
	
	return 0;
}


static struct fuse_operations operations = {
    .getattr	= do_getattr,
    .readdir	= do_readdir,
    .read		= do_read,
    .rename		=do_rename
};

int main( int argc, char *argv[] )
{
	int i;
	char *temp_path="";
	char line[LINE_LENGTH];
	size_t len=0;
	file = fopen("postal-codes.csv", "r");
	for(i=0;i<NUMBER_OF_ROWS;i++){
		fgets(line, LINE_LENGTH, file);
		parse(i,line);
	}
	fuse_main( argc, argv, &operations, NULL );
	fclose(file);
	return 0;
}
