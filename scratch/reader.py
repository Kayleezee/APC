import csv
import numpy as np
import copy

class Reader:
    """
    Main purpose of this class is to read a csv file
    and extract the lines, which contain a certain
    value.
    Returns a dictionary which keys are the colomns,
    which were not specified with the keywords.
    """
    def __init__(self, filename, delimiter, skipFileHeader=False):
        self.filename = filename
        self.f = open(filename, 'r')
        self.reader = csv.DictReader(self.f, delimiter=delimiter)

    """
    PURPOSE     - Gives an dictionary with sorted entries,
                  which fulfill a certain filter condition.
    PARAMETERS  - sorting_key: string of the column name in the
                  csv file you want to sort for.
                - filter_dictionary:
                    keys:  strings of the column names in the csv
                           file
                    value: omit every row in the csv file, which
                           does not contain this value(s) in the
                           key labeled column.
    RETURN      - dictionary:
                    keys:  string column name in the csv file
                    value: every entry in this column sorted
                           by column with name sorting_key,
                           which fulfills the filter condition.
    """
    def get_dict(self, sorting_key=None, filter_dic=dict()):
        rows = []
        for dic in self.reader:
            skip_row = False
            for key in filter_dic:
                entries = filter_dic[key]
                
                ## list of floats as filter ##
                if type(entries) == list and not type(entries[0]) == str:
                    skip_row = skip_row or not reduce(lambda x, y:\
                            x or round(y,10) == round(float(dic[key]),10), entries, False)
                
                ## one float as filter ##
                elif not type(entries) == str:
                    skip_row = skip_row or not round(float(dic[key]),10) == round(entries,10)
                
                ## string list as filter ##
                elif type(entries) == list and type(entries[0]) == str:
                    skip_row = skip_row or not reduce(lambda x, y:\
                            x or y == dic[key], entries, False)
                
                ## single string as filter ##
                elif type(entries) == str:
                    skip_row = skip_row or not dic[key] == entries
            
            if skip_row:
                continue
            else:
               rows.append(dic)
        
        ## sort entries if wanted ##
        if not sorting_key == None:
            rows = sorted(rows, key=lambda dic: float(dic[sorting_key]))
        
        ## write to output dictionary ##
        result = dict()
        for dic in rows:
            for key in dic:
                if not key in result:
                    result[key] = [dic[key]]
                else:
                    result[key].append(dic[key])
        
        ## reset file reader ##
        self.f.seek(0)
        
        ## convert entries to floats if possible ##
        for key in result:
            backup = copy.deepcopy(result[key])
            try:
                result[key] = map(float, result[key])
            except ValueError:
                result[key] = backup
        
        return result

    def get_header(self):
        return self.reader.fieldnames

    def close(self):
        self.f.close()
