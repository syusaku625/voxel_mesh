#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include<cmath>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

void export_part_vtu(const std::string &file, std::vector<std::vector<double>> node, std::vector<std::vector<int>> element)
{
    FILE *fp;
    if ((fp = fopen(file.c_str(), "w")) == NULL)
    {
        std::cout << file << " open error" << std::endl;
        exit(1);
    }
    int numOfelement = element.size();
    int numOfnode = node.size();
    fprintf(fp, "<VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\" header_type=\"UInt32\">\n");
    fprintf(fp, "<UnstructuredGrid>\n");
    fprintf(fp, "<Piece NumberOfPoints= \"%d\" NumberOfCells= \"%d\" >\n", numOfnode, numOfelement);
    fprintf(fp, "<Points>\n");
    int offset = 0;
    fprintf(fp, "<DataArray type=\"Float64\" Name=\"Position\" NumberOfComponents=\"3\" format=\"appended\" offset=\"%d\"/>\n", offset);
    offset += sizeof(int) + sizeof(double) * numOfnode * 3;
    fprintf(fp, "</Points>\n");

    fprintf(fp, "<Cells>\n");
    fprintf(fp, "<DataArray type=\"Int64\" Name=\"connectivity\" format=\"ascii\">\n");
    for (int i = 0; i < element.size(); i++)
    {
        for (int j = 0; j < element[i].size(); j++)
            fprintf(fp, "%d ", element[i][j]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "</DataArray>\n");
    fprintf(fp, "<DataArray type=\"Int64\" Name=\"offsets\" format=\"ascii\">\n");
    int num = 0;
    for (int i = 0; i < element.size(); i++)
    {
        num += element[i].size();
        fprintf(fp, "%d\n", num);
    }
    fprintf(fp, "</DataArray>\n");
    fprintf(fp, "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n");
    for (int i = 0; i < element.size(); i++)
    {
        if (element[i].size() == 4)
            fprintf(fp, "%d\n", 10);
        else if (element[i].size() == 6)
            fprintf(fp, "%d\n", 13);
        else if (element[i].size() == 10)
            fprintf(fp, "%d\n", 24);
        else if (element[i].size() == 15)
            fprintf(fp, "%d\n", 26);
        else if (element[i].size() == 8)
            fprintf(fp, "%d\n", 12);
    }
    fprintf(fp, "</DataArray>\n");
    fprintf(fp, "</Cells>\n");

    fprintf(fp, "<CellData>\n");
    fprintf(fp, "</CellData>\n");
    fprintf(fp, "</Piece>\n");
    fprintf(fp, "</UnstructuredGrid>\n");
    fprintf(fp, "<AppendedData encoding=\"raw\">\n");
    fprintf(fp, "_");
    fclose(fp);

    std::fstream ofs;
    ofs.open(file.c_str(), std::ios::out | std::ios::app | std::ios_base::binary);
    double *data_d = new double[numOfnode * 3];
    num = 0;
    int size = 0;
    for (int ic = 0; ic < numOfnode; ic++)
    {
        for (int j = 0; j < 3; j++)
        {
            data_d[num] = node[ic][j];
            num++;
        }
    }
    size = sizeof(double) * numOfnode * 3;
    ofs.write((char *)&size, sizeof(size));
    ofs.write((char *)data_d, size);

    delete data_d;
    ofs.close();

    if ((fp = fopen(file.c_str(), "a")) == NULL)
    {
        std::cout << file << " open error" << std::endl;
        exit(1);
    }
    fprintf(fp, "\n</AppendedData>\n");
    fprintf(fp, "</VTKFile>\n");
    fclose(fp);
}

int main(int argc,char *argv[])
{
    struct stat st;
    if(stat("output", &st) != 0){
         mkdir("output", 0775);
    }
    int num_node_x = 4;
    int num_node_y = 100;
    int num_elem = (num_node_x - 1) * (num_node_x - 1) * (num_node_y - 1);
    double voxel_d=0.1;
    vector<vector<double>> x(num_node_x * num_node_x * num_node_y, vector<double>(3));
    vector<vector<int>> element(num_elem);
    for (int i = 0; i < num_node_x; i++){
        for (int j = 0; j < num_node_x; j++){
            for (int k = 0; k < num_node_y; k++){
                x[k + num_node_y * j + (num_node_y * num_node_x)*i][0] = voxel_d * k;
                x[k + num_node_y * j + (num_node_y * num_node_x)*i][1] = voxel_d * j;
                x[k + num_node_y * j + (num_node_y * num_node_x)*i][2] = voxel_d * i;
                cout << k + num_node_y * j + (num_node_y * num_node_x)*i << " " << voxel_d * k << " " << voxel_d * j << " " << voxel_d * i << endl; 
            }
        }
    }
    vector<int> corner_set;
    for(int j=0; j<num_node_x-1; j++){
        for(int k=0; k<num_node_x-1; k++){
            for(int l=0; l<num_node_y-1; l++){
                int corner_node=l+k*(num_node_y)+j*(num_node_x*num_node_y);
                corner_set.push_back(corner_node);  
            }
        }
    }
    for(int i=0; i<element.size(); i++){
        element[i].push_back(corner_set[i]);
        element[i].push_back(corner_set[i]+1);
        element[i].push_back(corner_set[i]+1+num_node_y);
        element[i].push_back(corner_set[i]+num_node_y);

        element[i].push_back(corner_set[i]+num_node_y*num_node_x);
        element[i].push_back(corner_set[i]+num_node_y*num_node_x+1);
        element[i].push_back(corner_set[i]+num_node_y*num_node_x+1+num_node_y);
        element[i].push_back(corner_set[i]+num_node_y*num_node_x+num_node_y);
    }
    for(int i=0; i<element.size(); i++){
        for(int j=0; j<element[i].size(); j++){
            cout << element[i][j] << " ";
        }
        cout << endl;
    }
    export_part_vtu("output/test_shape.vtu", x, element);

    ofstream ofs("output/node.dat");
    for(int i=0; i<x.size(); i++){
        for(int j=0; j<3; j++){
            ofs << x[i][j] << " ";
        }
        ofs << endl;
    }
    ofs.close();

    ofs.open("output/element.dat");
    for(int i=0; i<element.size(); i++){
        for(int j=0; j<element[i].size(); j++){
            ofs << element[i][j] << " ";
        }
        ofs << endl;
    } 
    ofs.close();

    ofs.open("output/elementType.dat");
    for(int i=0; i<element.size(); i++){
        ofs << 12 << endl;
    }
    ofs.close();

    ofs.open("output/left_node.dat");
    for(int i=0; i<x.size(); i++){
        if(fabs(x[i][0]-0.0)<0.0000001) {
            ofs << i << endl;
        }
    }
    ofs.close();
    double x_max=0.0;
    for(int i=0; i<x.size(); i++){
        if(x_max<x[i][0]){
            x_max=x[i][0];
        }
    }

    ofs.open("output/right_node.dat");
    for(int i=0; i<x.size(); i++){
        if(fabs(x[i][0]-x_max)<0.0000001){
            ofs << i << endl;
        }
    }
    ofs.close();
}