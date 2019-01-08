#include "gui.h"
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#define MAX_PATH_LEN 512

struct file_info{
	char name[DXF_MAX_CHARS];
	time_t date;
};

int cmp_file_name(const void * a, const void * b) {
	struct file_info *info1 = ((struct sort_by_idx *)a)->data;
	struct file_info *info2 = ((struct sort_by_idx *)b)->data;
	
	return (strcmp(info1->name, info2->name));
}

int cmp_file_date(const void * a, const void * b) {
	struct file_info *info1 = ((struct sort_by_idx *)a)->data;
	struct file_info *info2 = ((struct sort_by_idx *)b)->data;
	
	return (int)round(difftime(info1->date, info2->date));
}

int file_win (gui_obj *gui){
	static char full_path[MAX_PATH_LEN];
	static char sel_file[MAX_PATH_LEN];
	
	struct dirent *entry;
	static DIR *work = NULL;
	DIR *subdir;
	char ext[4], *suffix, *end, dir[] = ".";
	char date[20];
	if (!work){
		work = opendir(dir);
		full_path[0] = 0;
		sel_file[0] = 0;
	}
	if (!work) return 0;
	
	
	
	int show_browser = 1;
	int i = 0;
	int idx = 0;
	
	struct stat filestat;
	struct tm *info;
	
	static struct sort_by_idx sort_dirs[10000];
	static struct sort_by_idx sort_files[10000];
	static struct file_info dirs[10000];
	static struct file_info files[10000];
	int num_files = 0, num_dirs = 0;
	
	gui->next_win_x += gui->next_win_w + 3;
	//gui->next_win_y += gui->next_win_h + 3;
	gui->next_win_w = 600;
	gui->next_win_h = 800;
	
	struct nk_style_button b_dir, b_file;
	b_dir = gui->ctx->style.button;
	b_file = gui->ctx->style.button;
	
	b_file.text_alignment = NK_TEXT_LEFT;
	b_dir.text_alignment = NK_TEXT_LEFT;
	b_dir.text_normal = nk_rgb(255,255,0);
	b_dir.text_hover = nk_rgb(255,255,0);
	b_dir.text_active = nk_rgb(255,255,0);
	
	if (nk_begin(gui->ctx, "File", nk_rect(gui->next_win_x, gui->next_win_y, gui->next_win_w, gui->next_win_h),
	NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
	NK_WINDOW_CLOSABLE|NK_WINDOW_TITLE)){
		rewinddir(work);
		
		while((entry = readdir(work))){
			subdir = opendir(entry->d_name);
			if (subdir != NULL){
				if (!(strcmp(entry->d_name, ".") == 0) && !(strcmp(entry->d_name, "..") == 0)){
					if (num_dirs < 10000){
						sort_dirs[num_dirs].idx = num_dirs;
						strncpy (dirs[num_dirs].name, entry->d_name, DXF_MAX_CHARS);
						stat(entry->d_name, &filestat);
						dirs[num_dirs].date = filestat.st_mtime;
						sort_dirs[num_dirs].data = &(dirs[num_dirs]);
						
						num_dirs++;
					}
				}
				
				closedir(subdir);
			}
			else{
				suffix = get_ext(entry->d_name);
				strncpy(ext, suffix, 4);
				ext[3] = 0; /*terminate string */
				str_upp(ext); /* upper case extension*/
				if (strcmp(ext, "DXF") == 0) {
					if (num_files < 10000){
						sort_files[num_files].idx = num_files;
						strncpy (files[num_files].name, entry->d_name, DXF_MAX_CHARS);
						stat(entry->d_name, &filestat);
						files[num_files].date = filestat.st_mtime;
						sort_files[num_files].data = &(files[num_files]);
						
						num_files++;
					}
				}
				
			}
		}
		
		static int sorted = 0;
		enum sort {
			UNSORTED,
			BY_NAME,
			BY_DATE
		};
		char curr_path[MAX_PATH_LEN];
		getcwd(curr_path, MAX_PATH_LEN);
		
		nk_layout_row_template_begin(gui->ctx, 22);
		nk_layout_row_template_push_dynamic(gui->ctx);
		nk_layout_row_template_push_static(gui->ctx, 30);
		nk_layout_row_template_end(gui->ctx);
		
		nk_label(gui->ctx, curr_path, NK_TEXT_LEFT);
		if (nk_button_label(gui->ctx,  "UP")){
			closedir(work);
			chdir("..");
			work = opendir(dir);
			if (!work) return 0;
		}
		
		nk_layout_row_dynamic(gui->ctx, 32, 1);
		if (nk_group_begin(gui->ctx, "file_head", NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR)) {
		
			nk_layout_row_template_begin(gui->ctx, 22);
			nk_layout_row_template_push_dynamic(gui->ctx);
			nk_layout_row_template_push_static(gui->ctx, 138);
			nk_layout_row_template_push_static(gui->ctx, 8);
			nk_layout_row_template_end(gui->ctx);
			
			/* sort by Layer name */
			if (sorted == BY_NAME){
				if (nk_button_symbol_label(gui->ctx, NK_SYMBOL_TRIANGLE_DOWN, "Name", NK_TEXT_CENTERED)){
					sorted = UNSORTED;
				}
			}
			else {
				if (nk_button_label(gui->ctx,  "Name")){
					sorted = BY_NAME;
				}
			}
			/* sort by color */
			if (sorted == BY_DATE){
				if (nk_button_symbol_label(gui->ctx, NK_SYMBOL_TRIANGLE_DOWN, "Date", NK_TEXT_CENTERED)){
					sorted = UNSORTED;
				}
			}
			else {
				if (nk_button_label(gui->ctx,  "Date")){
					sorted = BY_DATE;
				}
			}
			
			nk_group_end(gui->ctx);
		}
		nk_layout_row_dynamic(gui->ctx, 600, 1);
		if (nk_group_begin(gui->ctx, "File_view", NK_WINDOW_BORDER)) {
			//nk_layout_row_dynamic(gui->ctx, 20, 2);
			nk_layout_row_template_begin(gui->ctx, 20);
			nk_layout_row_template_push_dynamic(gui->ctx);
			nk_layout_row_template_push_static(gui->ctx, 138);
			nk_layout_row_template_end(gui->ctx);
			
			
			if (sorted == BY_NAME){
				qsort(sort_dirs, num_dirs, sizeof(struct sort_by_idx), cmp_file_name);
				qsort(sort_files, num_files, sizeof(struct sort_by_idx), cmp_file_name);
			}
			else if (sorted == BY_DATE){
				qsort(sort_dirs, num_dirs, sizeof(struct sort_by_idx), cmp_file_date);
				qsort(sort_files, num_files, sizeof(struct sort_by_idx), cmp_file_date);
			}
			
			for (i = 0; i < num_dirs; i++){
				idx = sort_dirs[i].idx;
				if (nk_button_label_styled(gui->ctx, &b_dir,  dirs[idx].name)){
					closedir(work);
					chdir(dirs[idx].name);
					work = opendir(dir);
					full_path[0] = 0;
					sel_file[0] = 0;
					if (!work) return 0;
				}
				
				//nk_label_colored(gui->ctx, entry->d_name, NK_TEXT_LEFT, nk_rgb(255,255,0));
				
				info = localtime(&(dirs[idx].date));
				snprintf(date, 20, "%02d/%02d/%04d %02d:%02d:%02d", info->tm_mday, info->tm_mon+1, info->tm_year+1900, info->tm_hour, info->tm_min, info->tm_sec);
				nk_label_colored(gui->ctx, date, NK_TEXT_LEFT, nk_rgb(255,255,0));
			}
			for (i = 0; i < num_files; i++){
				idx = sort_files[i].idx;
				if (nk_button_label_styled(gui->ctx, &b_file,  files[idx].name)){
					strncpy(sel_file, files[idx].name, MAX_PATH_LEN);
				}
				info = localtime(&(files[idx].date));
				snprintf(date, 20, "%02d/%02d/%04d %02d:%02d:%02d", info->tm_mday, info->tm_mon+1, info->tm_year+1900, info->tm_hour, info->tm_min, info->tm_sec);
				nk_label(gui->ctx, date, NK_TEXT_LEFT);
			}
			
			nk_group_end(gui->ctx);
			
			if (strlen(sel_file) > 0){
				snprintf(full_path, MAX_PATH_LEN, "%s%c%s", curr_path, DIR_SEPARATOR, sel_file);
			}
			
			nk_layout_row_dynamic(gui->ctx, 20, 1);
			nk_label_colored(gui->ctx, "Selected:", NK_TEXT_LEFT, nk_rgb(0,0,255));
			nk_label_colored(gui->ctx, sel_file, NK_TEXT_LEFT, nk_rgb(255,0,255));
			nk_label(gui->ctx, full_path, NK_TEXT_LEFT);
		}
		
	} else {
		closedir(work);
		work = NULL;
		full_path[0] = 0;
		sel_file[0] = 0;
		
		show_browser = 0;
	}
	nk_end(gui->ctx);
	
	return show_browser;
}