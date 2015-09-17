<?php
/**
 * @file
 * Implement context condition class for the assigned Workbench Access section
 */

/**
 * Expose the Workbench Access Section as a context condition.
 */
class iit_condition_workbench_access extends context_condition {

    /**
     * @return array
     */
    function condition_values() {
        $values = array();

        if (module_exists('taxonomy')) {
            $wa_vocabs = $this->_get_workbench_access_vocabulary_names();

            foreach ($wa_vocabs as $vocab_name) {
                $vocab = taxonomy_vocabulary_machine_name_load($vocab_name);
                foreach (taxonomy_get_tree($vocab->vid) as $term) {
                    $values[$term->tid] = check_plain($term->name);
                }
            }
        }

        return $values;
    }

    /**
     * @param $context
     * @return array
     */
    function condition_form($context) {
        $form = parent::condition_form($context);
        $form['#type'] = 'select';
        $form['#size'] = 12;
        $form['#multiple'] = TRUE;
        $options = array();

        $wa_vocabs = $this->_get_workbench_access_vocabulary_names();
        foreach ($wa_vocabs as $vocab_name ) {
            $vocab = taxonomy_vocabulary_machine_name_load($vocab_name);
            $tree = taxonomy_get_tree($vocab->vid);
            if ($tree && (count($tree) > 0)) {
                $options[$vocab->name] = array();
                foreach ($tree as $term) {
                    $options[$vocab->name][$term->tid] = str_repeat('-', $term->depth) . $term->name;
                }
            }
        }

        $form['#options'] = $options;
        return $form;
    }

    /**
     * @param $node
     */
    function execute($node) {
        if ($this->condition_used() && !empty($node->workbench_access)) {
            foreach ($node->workbench_access as $access_id) {
                foreach ($this->get_contexts($access_id) as $context) {
                    $this->condition_met($context, $access_id);
                }
            }
        }
    }

    /**
     * @return array
     */
    private function _get_workbench_access_vocabulary_names() {
        $enabled_wa_vocabularies = variable_get('workbench_access_taxonomy', array());
        $wa_vocabs = array();
        foreach ($enabled_wa_vocabularies as $val) {
            if (is_string($val)) {
                $wa_vocabs[] = $val;
            }
        }
        return $wa_vocabs;
    }

}