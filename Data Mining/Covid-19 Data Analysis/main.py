import math
from mlxtend.frequent_patterns import apriori
from mlxtend.frequent_patterns import association_rules
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd


def mapping(x):
    if x == "N/A":
        return 0


def perc_form(x):
    return str(np.round(x, 2)) + "%"


# data frames
def rule_mining(fn: str, impacted):
    df = pd.read_csv(fn + ".csv")
    print(df.head())

    # make graphs on data already
    b_gen = df["BIRTH GENDER"].value_counts(normalize=True)
    print(b_gen)
    plt.pie(x=b_gen, autopct=perc_form, explode=[0.1, 0.1])
    plt.title("Birth Gender Ratio")
    plt.legend(["Female", "Male"], loc=3)
    plt.show()

    c_gen = df["CURRENT GENDER"].value_counts(normalize=True)
    print(c_gen)
    plt.pie(x=c_gen, autopct=perc_form, explode=[0.1, 0.1, 0.1, 0.4])
    plt.title("Current Gender Ratio")
    plt.legend(["Female", "Male", "Other", "Trans"], loc=3)
    plt.show()

    race = df["RACE"].value_counts(normalize=True)
    print(race)
    plt.pie(x=race, autopct=perc_form, explode=[0.1, 0.1, 0.1, 0.1, 0.1])
    plt.title("Ethnicity Ratio")
    plt.legend(["White", "Hispanic", "Black", "Mixed", "Asian"], loc=3)
    plt.show()

    age = df["AGE"].value_counts(normalize=True)
    print(age)
    plt.scatter(y=age.values, x=age.index)
    plt.ylabel("Percentage")
    plt.xlabel("Age")
    plt.title("Distribution of Covid Patient Ages")
    plt.show()

    lc = df["LONG COVID"].value_counts(normalize=True)
    plt.pie(x=lc, autopct=perc_form, explode=[0.1, 0.1])
    plt.legend(["No", "Yes"], loc=3)
    plt.title("Has Long Covid")
    plt.show()
    print(lc)

    sy = df["SYMPTOM SEVERITY"].value_counts(normalize=True)
    plt.pie(x=sy, autopct=perc_form, explode=[0.1, 0.1, 0.1, 0.1])
    plt.legend(["Moderate", "Mild", "Severe", "None"], loc=3)
    plt.title("Symptom Severity Among Covid Patients")
    plt.show()
    print(sy)

    va = df["VACCINATED"].value_counts(normalize=True)
    plt.pie(x=va, autopct=perc_form, explode=[0.1, 0.1])
    plt.title("Vaccination Rate Among Covid Patients")
    plt.legend(["Vaccinated", "Unvaccinated"], loc=3)
    plt.show()
    print(va)

    # Break-downs for long covid patients
    # take the long-covid patients
    lc_filtered = df[df["LONG COVID"].isin([1])]
    # long-covid break-down by birth gender
    lc_g = lc_filtered["BIRTH GENDER"].value_counts(normalize=True)
    plt.pie(x=lc_g, autopct=perc_form, explode=[0.1, 0.1])
    plt.title("Long Covid Patients by Birth Gender")
    plt.legend(["Female", "Male"], loc=3)
    plt.show()
    print(lc_g)

    lc_cg = lc_filtered["CURRENT GENDER"].value_counts(normalize=True)
    plt.pie(x=lc_cg, autopct=perc_form, explode=[0.1, 0.1, 0.1, 0.4])
    plt.title("Long Covid Patients by Current Gender")
    plt.legend(["Female", "Male", "Other", "Trans"], loc=3)
    plt.show()
    print(lc_cg)

    # long-covid break-down by race
    lc_r = lc_filtered["RACE"].value_counts(normalize=True)
    plt.pie(x=lc_r, autopct=perc_form, explode=[0.1, 0.1, 0.1, 0.1, 0.1])
    plt.title("Long Covid Patients by Ethnicity")
    plt.legend(["White", "Hispanic", "Black", "Mixed", "Asian"], loc=3)
    plt.show()
    print(lc_r)

    # long-covid break-down by vaccination
    lc_v = lc_filtered["VACCINATED"].value_counts(normalize=True)
    plt.pie(x=lc_v, autopct=perc_form, explode=[0.1, 0.1])
    plt.title("Long Covid Patients by Vaccination")
    plt.legend(["Vaccinated", "Unvaccinated"], loc=3)
    plt.show()
    print(lc_v)

    # long-covid break-down by symptom severity
    lc_s = lc_filtered["SYMPTOM SEVERITY"].value_counts(normalize=True)
    plt.pie(x=lc_s, autopct=perc_form, explode=[0.1, 0.1, 0.1, 0.1])
    plt.title("Long Covid Patients by Symptom Severity")
    plt.legend(["Moderate", "Severe", "Mild", "None"], loc=3)
    plt.show()
    print(lc_s)

    # long-covid break-down by age
    lc_a = lc_filtered["AGE"].value_counts(normalize=True)
    plt.scatter(y=lc_a.values, x=lc_a.index)
    plt.ylabel("Percentage")
    plt.xlabel("Age")
    plt.title("Distribution of Long Covid Patient Ages")
    plt.show()

    # calculate minsup according to the size of the input dataframe, and some constants
    # -0.4, -0.2, +0.1
    minsup = math.exp(-0.4 * (len(df.index)) - 0.2) + 0.1

    # convert N/A & NO to 0, and YES to 1, else, leave it
    df.applymap(func=lambda x: 0 if "NO" else 1 if "YES" else 0 if "N/A" else x)

    # Convert input dataframe into data that can be mined by apriori
    # Split age into age range
    df['AGE RANGE'] = pd.qcut(df['AGE'], q=4, labels=['<=36', '>36 &  <=54', '>54 & <=72', '>72'])
    # split number doses into range
    # df['DOSE RANGE'] = pd.qcut(df['NUMBER DOSES'], duplicates="drop", q=3, labels=['1 DOSE', '2 DOSES'])

    """# Split symptom severity into range
    df['SYMPTOM SEVERITY RANGE'] = pd.qcut(df['SYMPTOM SEVERITY'], q=4, labels=["MODERATE", 'MILD', 'SEVERE', 'NONE'])
    # Split race into range
    df['RACE RANGE'] = pd.qcut(df['RACE'], q=5, labels=['White', 'Black', 'Asian', 'Hispanic', 'Mixed'])
    # split current gender into range
    df['GENDER RANGE'] = pd.qcut(df['CURRENT GENDER'], q=4, labels=['M', 'F', 'T', 'O'])
    
    # split impacted range
    df['IMPACTED RANGE'] = pd.qcut(df['IMPACTED'], q=3, labels=['A LOT', 'A LITTLE', 'NONE'])"""

    # remove unused columns in favor for split ones
    if impacted:
        columns = ['AGE RANGE', 'SYMPTOM SEVERITY', 'RACE', 'BIRTH GENDER', 'CURRENT GENDER', 'VACCINATED', 'LONG COVID',
                   'IMPACTED', 'BOOSTER', 'NUMBER DOSES', 'TREAT ORAL', 'TREAT MONO', 'CURRENT SYMPTOMS']
    else:
        columns = ['AGE RANGE', 'SYMPTOM SEVERITY', 'RACE', 'BIRTH GENDER', 'CURRENT GENDER', 'VACCINATED',
                   'LONG COVID', 'BOOSTER', 'NUMBER DOSES', 'TREAT ORAL', 'TREAT MONO', 'CURRENT SYMPTOMS']
    unused = list(set(df.columns.to_list()) - set(columns))
    # get_dummies splits categorical data into multiple columns
    # ie -> ['M', 'F'] becomes [[1, 0], [0, 1]]
    df = pd.get_dummies(df, columns=columns)
    df.drop(labels=unused, axis=1, inplace=True)
    frequent = apriori(df, minsup, True, max_len=5)

    # print(frequent)

    rules = association_rules(frequent, metric="confidence", min_threshold=0.3)

    # look for long covid in consiquents

    # Extract strings from frozensets in dataframe, leaving raw strings in csv
    rules["antecedents"] = rules["antecedents"].apply(lambda x: ', '.join(list(x))).astype("unicode")
    rules["consequents"] = rules["consequents"].apply(lambda x: ', '.join(list(x))).astype("unicode")
    # Extract strings from frozensets in dataframe, leaving raw strings in dataframe
    frequent["itemsets"] = frequent["itemsets"].apply(lambda x: ', '.join(list(x))).astype("unicode")

    filter = ["LONG COVID_0", 'LONG COVID_1']
    filtered = rules[rules["consequents"].isin(filter)]
    filtered = filtered.sort_values(by='confidence')
    filtered.reset_index(inplace=True, drop=True)
    print(filtered.head(10))

    lc_yes = rules[rules["consequents"].isin(["LONG COVID_1"])]
    lc_yes = lc_yes.sort_values(by='confidence')
    lc_yes.reset_index(inplace=True, drop=True)
    lc_no = rules[rules["consequents"].isin(["LONG COVID_0"])]
    lc_no = lc_no.sort_values(by='confidence')
    lc_no.reset_index(inplace=True, drop=True)

    plt.bar(x=range(0, lc_yes.shape[0]), height=lc_yes["confidence"])
    plt.title("Confidence with 'Long-Covid Occurring' as Consequent")
    plt.xlabel("Rule Index")
    plt.ylabel("Confidence")
    plt.show()
    plt.rcParams['figure.dpi'] = 200
    plt.bar(x=range(0, lc_no.shape[0]), height=lc_no["confidence"])
    plt.title("Confidence with 'Long-Covid not Occurring' as Consequent")
    plt.xlabel("Rule Index")
    plt.ylabel("Confidence")
    plt.show()

    # output data to csv
    out_name = fn + "-output.csv"
    filtered.to_csv(out_name)
    lc_yes.to_csv(fn + "-LC.csv")
    lc_no.to_csv(fn + "-NoLC.csv")


# rule_mining("KenyaDataEdited")
# rule_mining("MalawiDataEdited")
rule_mining("US_Week49_COVID", true)
rule_mining("US_WEEK46_COVID", false)
# rule_mining(input("File name (w/out extension)"))
